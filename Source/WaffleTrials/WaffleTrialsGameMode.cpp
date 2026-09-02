// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaffleTrialsGameMode.h"
#include "ItemType.h"
#include "WGameInstance.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "WaffleTrialsGameState.h"
#include "WaffleTrials.h"

AWaffleTrialsGameMode::AWaffleTrialsGameMode()
{
	// stub
}

void AWaffleTrialsGameMode::BeginPlay() {
	Super::BeginPlay();

	buildItemPool();

	// subscribe to gameOver signal
	if (AWaffleTrialsGameState* gameState = GetGameState<AWaffleTrialsGameState>())
		gameState->onGameOver.AddUObject(this, &AWaffleTrialsGameMode::onGameOver);

	slotTimers.SetNum(orderSlotCount);
	for (int32 i = 0; i < orderSlotCount; ++i)
		startNewOrderTimer(i);
}

void AWaffleTrialsGameMode::buildItemPool(){
	// better to cache them then access the table every time
	UWGameInstance* gameInstance = Cast<UWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!gameInstance || !gameInstance->ItemDataTable){
		UE_LOG(LogWaffleTrials, Error, TEXT("no game instance or data table"));
		return;
	}

	const UEnum* itemEnum = StaticEnum<EItem>();

	for (int32 i = 1; i < itemEnum->NumEnums() - 1; ++i){
		const EItem item = static_cast<EItem>(itemEnum->GetValueByIndex(i));

		const FString itemName = UEnum::GetDisplayValueAsText(item).ToString();
		const FItemData* itemData = gameInstance->ItemDataTable->FindRow<FItemData>(
			FName(*itemName), TEXT("building pool"));

		if (!itemData){
			continue;
		}

		if (itemData->servable){
			servableItems.Add(item);
			itemSeconds.Add(item, itemData->seconds);
		}
	}
}

void AWaffleTrialsGameMode::onOrderCompleted(int32 slotIndex){
	startNewOrderTimer(slotIndex);
}

void AWaffleTrialsGameMode::startNewOrderTimer(int32 slotIndex){
	if (!slotTimers.IsValidIndex(slotIndex)) return;

	const float time = FMath::FRandRange(minWait, maxWait);

	FTimerDelegate del = FTimerDelegate::CreateUObject(
		this, &AWaffleTrialsGameMode::spawnOrder, slotIndex);

	GetWorldTimerManager().SetTimer(slotTimers[slotIndex], del, time, false);
}

void AWaffleTrialsGameMode::spawnOrder(int32 slotIndex){
	if (servableItems.Num() == 0){
		UE_LOG(LogWaffleTrials, Error, TEXT("no servable items, cant spawn order"));
		return;
	}

	AWaffleTrialsGameState* gameState = GetGameState<AWaffleTrialsGameState>();
	if (!gameState){
		UE_LOG(LogWaffleTrials, Error, TEXT("no gamestate"));
		return;
	}

	FOrder newOrder;
	newOrder.orderId = nextOrderId++;

	const int32 itemCount = FMath::RandRange(1, 3);
	newOrder.items.SetNum(itemCount);

	for (FOrderItem& orderItem : newOrder.items){
		orderItem.item = servableItems[FMath::RandRange(0, servableItems.Num() - 1)];
		orderItem.delivered = false;
	}

	float total = 0.f;
	for (const FOrderItem& orderItem : newOrder.items)
		total += itemSeconds.FindRef(orderItem.item);

	newOrder.timeLimit = gameState->GetServerWorldTimeSeconds() + total;
	newOrder.startTime = gameState->GetServerWorldTimeSeconds();

	FTimerDelegate del = FTimerDelegate::CreateUObject(
		this, &AWaffleTrialsGameMode::onOrderTimeLimitReached, slotIndex);
	GetWorldTimerManager().SetTimer(slotTimers[slotIndex], del, total, false);

	gameState->setOrder(slotIndex, newOrder);

	UE_LOG(LogWaffleTrials, Warning, TEXT("spawned order %d in slot %d, %d items"),
		newOrder.orderId, slotIndex, itemCount);
}

void AWaffleTrialsGameMode::onOrderTimeLimitReached(int32 slotIndex){
	AWaffleTrialsGameState* gameState = GetGameState<AWaffleTrialsGameState>();
	if (!gameState)
		return;

	gameState->clearOrder(slotIndex);
	gameState->loseLife();

	// dont start the timer if the game is over
	if (gameState->isGameOver())
		return;
	startNewOrderTimer(slotIndex);
}

void AWaffleTrialsGameMode::onGameOver(){
	// need to stop all the order timers
	for (FTimerHandle& handle : slotTimers){
		GetWorldTimerManager().ClearTimer(handle);
	}
}

void AWaffleTrialsGameMode::resetOrderTimer(int32 slotIndex, float newTime) {
	if (!slotTimers.IsValidIndex(slotIndex)) return;

	FTimerDelegate del = FTimerDelegate::CreateUObject(
		this, &AWaffleTrialsGameMode::onOrderTimeLimitReached, slotIndex);
	GetWorldTimerManager().SetTimer(slotTimers[slotIndex], del, newTime, false);
}