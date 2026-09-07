
#include "WGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "WaffleTrials.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

static const FName SESSION_NAME = TEXT("WaffleSession");
static const FName KEY_GAMEID = TEXT("WAFFLETRIALS_GAMEID");
static const FString GAMEID_VALUE = TEXT("WaffleTrialsV1");

static void screenMsg(const FString& text, FColor colour = FColor::Green) {
	if (!GEngine)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 10.f, colour, text);
}

void UWGameInstance::Init() {
	Super::Init();

	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	if (!subsystem) {
		screenMsg(TEXT("no online subsystem"), FColor::Red);
		return;
	}

	screenMsg(FString::Printf(TEXT("subsystem: %s"), *subsystem->GetSubsystemName().ToString()));

	sessionInterface = subsystem->GetSessionInterface();
	if (!sessionInterface.IsValid()) {
		screenMsg(TEXT("session interface invalid"), FColor::Red);
		return;
	}

	screenMsg(TEXT("session interface valid"));

	sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UWGameInstance::onCreateComplete);
	sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UWGameInstance::onFindComplete);
	sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UWGameInstance::onJoinComplete);
	sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UWGameInstance::onDestroyComplete);
}

void UWGameInstance::hostGame() {
	screenMsg(TEXT("hostGame"));

	if (!sessionInterface.IsValid())
		return;

	if (hosting)
		return;

	hosting = true;

	FNamedOnlineSession* stale = sessionInterface->GetNamedSession(SESSION_NAME);
	if (!stale) {
		createSession();
		return;
	}

	// the create has to happen in onDestroyComplete, otherwise it fires
	// while the old session is still around and fails
	screenMsg(TEXT("destroying session"));
	sessionInterface->DestroySession(SESSION_NAME);
}

void UWGameInstance::createSession() {
	if (!sessionInterface.IsValid())
		return;

	FOnlineSessionSettings settings;
	settings.bIsLANMatch = useLAN;
	settings.NumPublicConnections = 2;
	settings.bShouldAdvertise = true;
	settings.bUsesPresence = !useLAN;
	settings.bUseLobbiesIfAvailable = !useLAN;
	settings.bAllowJoinInProgress = true;

	settings.Set(KEY_GAMEID, GAMEID_VALUE, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	bool started = sessionInterface->CreateSession(0, SESSION_NAME, settings);
}

void UWGameInstance::onDestroyComplete(FName sessionName, bool success) {
	screenMsg(FString::Printf(TEXT("onDestroyComplete success=%d"), success), FColor::Cyan);

	if (!success)
		return;

	createSession();
}

void UWGameInstance::onCreateComplete(FName sessionName, bool success) {
	screenMsg(FString::Printf(TEXT("onCreateComplete success=%d"), success), FColor::Cyan);

	if (!success)
		return;

	hosting = false;

	GetWorld()->ServerTravel(lobbyMapName + TEXT("?listen"));
}

void UWGameInstance::findGames() {
	screenMsg(TEXT("findGames"));

	if (!sessionInterface.IsValid())
		return;

	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	sessionSearch->MaxSearchResults = 100;
	sessionSearch->bIsLanQuery = useLAN;

	if (!useLAN)
		sessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), true, EOnlineComparisonOp::Equals);

	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UWGameInstance::onFindComplete(bool success) {
	screenMsg(FString::Printf(TEXT("onFindComplete success=%d"), success), FColor::Yellow);

	if (!success)
		return;
	if (!sessionSearch.IsValid())
		return;

	screenMsg(FString::Printf(TEXT("raw results: %d"), sessionSearch->SearchResults.Num()), FColor::Yellow);

	sessionSearch->SearchResults.RemoveAll([](const FOnlineSessionSearchResult& result) {
		FString value;
		bool found = result.Session.SessionSettings.Get(KEY_GAMEID, value);
		if (!found)
			return true;

		return value != GAMEID_VALUE;
		});

	screenMsg(FString::Printf(TEXT("waffle results: %d"), sessionSearch->SearchResults.Num()), FColor::Yellow);

	onGamesFound.Broadcast();
}

int32 UWGameInstance::getFoundCount() const {
	if (!sessionSearch.IsValid())
		return 0;

	return sessionSearch->SearchResults.Num();
}

void UWGameInstance::joinFoundGame(int32 index) {
	screenMsg(FString::Printf(TEXT("joinFoundGame %d"), index));

	if (!sessionInterface.IsValid())
		return;
	if (!sessionSearch.IsValid())
		return;

	// silent failure here cost us an afternoon, so it says so out loud now
	if (!sessionSearch->SearchResults.IsValidIndex(index)) {
		screenMsg(FString::Printf(TEXT("bad index %d, have %d"), index, sessionSearch->SearchResults.Num()), FColor::Red);
		return;
	}

	sessionInterface->JoinSession(0, SESSION_NAME, sessionSearch->SearchResults[index]);
}

void UWGameInstance::onJoinComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result) {
	screenMsg(FString::Printf(TEXT("onJoinComplete result=%d"), (int32)result), FColor::Cyan);

	if (result != EOnJoinSessionCompleteResult::Success)
		return;

	FString address;
	bool resolved = sessionInterface->GetResolvedConnectString(sessionName, address);
	if (!resolved) {
		screenMsg(TEXT("no connect string"), FColor::Red);
		return;
	}

	screenMsg(FString::Printf(TEXT("travelling to %s"), *address));

	APlayerController* pc = GetFirstLocalPlayerController();
	if (!pc)
		return;

	pc->ClientTravel(address, ETravelType::TRAVEL_Absolute);
}

TArray<FFoundGame> UWGameInstance::getFoundGames() const {
	TArray<FFoundGame> games;

	if (!sessionSearch.IsValid())
		return games;

	for (int32 i = 0; i < sessionSearch->SearchResults.Num(); i++) {
		const FOnlineSessionSearchResult& result = sessionSearch->SearchResults[i];

		FFoundGame game;
		game.hostName = result.Session.OwningUserName;
		game.ping = result.PingInMs;
		game.openSlots = result.Session.NumOpenPublicConnections;
		game.maxSlots = result.Session.SessionSettings.NumPublicConnections;
		game.index = i;

		games.Add(game);
	}

	return games;
}

bool UWGameInstance::isHost() const {
	UWorld* world = GetWorld();
	if (!world)
		return false;

	ENetMode mode = world->GetNetMode();
	// makes sure this only works with the host and in single player
	// NM_Client would allow the other players to restart the game
	// which we don't want
	return mode == NM_ListenServer || mode == NM_Standalone;
}

void UWGameInstance::restartGame() {
	if (!isHost())
		return;

	GetWorld()->ServerTravel(lobbyMapName + TEXT("?listen"));
}

void UWGameInstance::quitGame() {
	// ends the game for everyone
	// note that this does start another session as its quitting
	if (sessionInterface.IsValid() && sessionInterface->GetNamedSession(SESSION_NAME))
		sessionInterface->DestroySession(SESSION_NAME);

	APlayerController* pc = GetFirstLocalPlayerController();
	if (!pc)
		return;

	UKismetSystemLibrary::QuitGame(this, pc, EQuitPreference::Quit, false);
}