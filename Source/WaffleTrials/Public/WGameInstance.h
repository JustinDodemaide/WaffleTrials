// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FindSessionsCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WAFFLETRIALS_API UWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Data")
	TObjectPtr<class UDataTable> ItemDataTable;


	UFUNCTION(BlueprintCallable, Category = "Session")
	void hostGame();

	UFUNCTION(BlueprintCallable, Category = "Session")
	void findGames();

	UFUNCTION(BlueprintCallable, Category = "Session")
	void joinFoundGame(int32 index);

	UPROPERTY(EditDefaultsOnly, Category = "Session")
	FString lobbyMapName = TEXT("YourMapName");

protected:
	IOnlineSessionPtr sessionInterface;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	void onCreateComplete(FName sessionName, bool success);
	void onFindComplete(bool success);
	void onJoinComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
};
