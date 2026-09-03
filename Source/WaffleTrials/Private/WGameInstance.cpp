// Fill out your copyright notice in the Description page of Project Settings.

#include "WGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "WaffleTrials.h"
#include "Online/OnlineSessionNames.h"

static const FName SESSION_NAME = TEXT("WaffleSession");
static const FName KEY_GAMEID = TEXT("WAFFLETRIALS_GAMEID");
static const FString GAMEID_VALUE = TEXT("WaffleTrialsV1");

void UWGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	if (!subsystem)
	{
		UE_LOG(LogWaffleTrials, Error, TEXT("no online subsystem"));
		return;
	}

	UE_LOG(LogWaffleTrials, Warning, TEXT("online subsystem: %s"), *subsystem->GetSubsystemName().ToString());

	sessionInterface = subsystem->GetSessionInterface();
	if (!sessionInterface.IsValid()) return;

	sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UWGameInstance::onCreateComplete);
	sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UWGameInstance::onFindComplete);
	sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UWGameInstance::onJoinComplete);
}

void UWGameInstance::hostGame()
{
	if (!sessionInterface.IsValid()) return;

	// destroy any stale session first
	if (sessionInterface->GetNamedSession(SESSION_NAME))
	{
		sessionInterface->DestroySession(SESSION_NAME);
	}

	FOnlineSessionSettings settings;
	settings.bIsLANMatch = false;
	settings.NumPublicConnections = 2;
	settings.bShouldAdvertise = true;
	settings.bUsesPresence = true;
	settings.bUseLobbiesIfAvailable = true;
	settings.bAllowJoinInProgress = true;

	settings.Set(KEY_GAMEID, GAMEID_VALUE, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	sessionInterface->CreateSession(0, SESSION_NAME, settings);
}

void UWGameInstance::onCreateComplete(FName sessionName, bool success)
{
	UE_LOG(LogWaffleTrials, Warning, TEXT("create session: %d"), success);
	if (!success) return;

	GetWorld()->ServerTravel(lobbyMapName + TEXT("?listen"));
}

void UWGameInstance::findGames()
{
	if (!sessionInterface.IsValid()) return;

	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	sessionSearch->MaxSearchResults = 100;
	sessionSearch->bIsLanQuery = false;
	sessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), true, EOnlineComparisonOp::Equals);
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UWGameInstance::onFindComplete(bool success)
{
	if (!success || !sessionSearch.IsValid()) return;

	// app id 480 is shared, so filter to only our sessions
	sessionSearch->SearchResults.RemoveAll([](const FOnlineSessionSearchResult& result)
		{
			FString value;
			return !result.Session.SessionSettings.Get(KEY_GAMEID, value) || value != GAMEID_VALUE;
		});

	UE_LOG(LogWaffleTrials, Warning, TEXT("found %d waffle sessions"), sessionSearch->SearchResults.Num());
}

void UWGameInstance::joinFoundGame(int32 index)
{
	if (!sessionInterface.IsValid() || !sessionSearch.IsValid()) return;
	if (!sessionSearch->SearchResults.IsValidIndex(index)) return;

	sessionInterface->JoinSession(0, SESSION_NAME, sessionSearch->SearchResults[index]);
}

void UWGameInstance::onJoinComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (result != EOnJoinSessionCompleteResult::Success) return;

	FString address;
	if (!sessionInterface->GetResolvedConnectString(sessionName, address)) return;

	APlayerController* pc = GetFirstLocalPlayerController();
	if (pc) pc->ClientTravel(address, ETravelType::TRAVEL_Absolute);
}
