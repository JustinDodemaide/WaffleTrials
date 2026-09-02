// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ItemType.h"
#include "WaffleTrialsGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AWaffleTrialsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AWaffleTrialsGameMode();

	// called by gamestate
	void onOrderCompleted(int32 slotIndex);

protected:
	virtual void BeginPlay() override;
	// dont want the customer to ask for raw eggs so we need a list of valid items
	TArray<EItem> servableItems;
	void buildItemPool();

	TMap<EItem, float> itemSeconds;

	UPROPERTY(EditDefaultsOnly, Category = "Orders")
	int32 orderSlotCount = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Orders")
	float minWait = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Orders")
	float maxWait = 10.f;

	TArray<FTimerHandle> slotTimers;

	void startNewOrderTimer(int32 slotIndex);
	void spawnOrder(int32 slotIndex);

	int32 nextOrderId = 0;

	void onOrderTimeLimitReached(int32 slotIndex);

	void onGameOver();
};



