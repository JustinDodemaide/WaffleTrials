// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FindSessionsCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FFoundGame {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString hostName;

	UPROPERTY(BlueprintReadOnly)
	int32 ping = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 openSlots = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 maxSlots = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 index = -1;
};

UCLASS()
class WAFFLETRIALS_API UWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// we only want the host to be able to restart (avoids an extra RPC)
	UFUNCTION(BlueprintPure, Category = "Session")
	bool isHost() const;

	UFUNCTION(BlueprintCallable, Category = "Session")
	void restartGame();

	UFUNCTION(BlueprintCallable, Category = "Session")
	void quitGame();

	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Data")
	TObjectPtr<class UDataTable> ItemDataTable;

	UFUNCTION(BlueprintCallable, Category = "Session")
	TArray<FFoundGame> getFoundGames() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamesFound);

	UPROPERTY(BlueprintAssignable, Category = "Session")
	FOnGamesFound onGamesFound;

public:
	UFUNCTION(BlueprintCallable, Category = "Session")
	void hostGame();

	UFUNCTION(BlueprintCallable, Category = "Session")
	void findGames();

	UFUNCTION(BlueprintCallable, Category = "Session")
	void joinFoundGame(int32 index);

	int32 getFoundCount() const;

	UPROPERTY(EditDefaultsOnly, Category = "Session")
	FString lobbyMapName = TEXT("Lvl_ThirdPerson");

	// i dont want to make a second steam account rn so testing on lan
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	bool useLAN = true;

protected:
	bool hosting = false;

	IOnlineSessionPtr sessionInterface;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	void createSession();

	void onCreateComplete(FName sessionName, bool success);
	void onFindComplete(bool success);
	void onJoinComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void onDestroyComplete(FName sessionName, bool success);
};