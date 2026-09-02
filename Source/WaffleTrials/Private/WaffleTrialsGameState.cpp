// Fill out your copyright notice in the Description page of Project Settings.


#include "WaffleTrials/WaffleTrialsGameMode.h"
#include "WaffleTrialsGameState.h"
#include "Net/UnrealNetwork.h"

void AWaffleTrialsGameState::BeginPlay(){
	Super::BeginPlay();
	if (!HasAuthority())
		return;

	currentLives = startingLives;
}

void AWaffleTrialsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWaffleTrialsGameState, orders);
	DOREPLIFETIME(AWaffleTrialsGameState, currentLives);
	DOREPLIFETIME(AWaffleTrialsGameState, gameOver);
}

bool AWaffleTrialsGameState::attemptSubmitItem(int32 slotIndex, EItem Item){
	if (!HasAuthority())
		return false;
	if (!orders.IsValidIndex(slotIndex))
		return false;

	FOrder& Order = orders[slotIndex];
	if (!Order.active())
		return false;

	for (FOrderItem& OrderItem : Order.items){
		if (OrderItem.item != Item)
			continue;
		if (OrderItem.delivered)
			continue;

		OrderItem.delivered = true;

		if (Order.fulfilled()){
			Order.orderId = -1;
			Order.items.Empty();

			if (AWaffleTrialsGameMode* gm = GetWorld()->GetAuthGameMode<AWaffleTrialsGameMode>()){
				gm->onOrderCompleted(slotIndex);
			}
		}
		// the logic for resetting the timer (round up to the nearest half)
		else{
			const float now = GetServerWorldTimeSeconds();
			const float duration = Order.timeLimit - Order.startTime;
			const float progress = FMath::Clamp((Order.timeLimit - now) / duration, 0.f, 1.f);

			const float newProgress = FMath::CeilToFloat(progress * 2.f) / 2.f;
			const float newRemaining = duration * newProgress;

			Order.startTime = now - (duration - newRemaining);
			Order.timeLimit = now + newRemaining;

			if (AWaffleTrialsGameMode* gm = GetWorld()->GetAuthGameMode<AWaffleTrialsGameMode>())
				gm->resetOrderTimer(slotIndex, newRemaining);
		}

		onOrdersChanged.Broadcast();
		return true;
	}

	return false;
}


void AWaffleTrialsGameState::OnRep_ActiveOrders(){
	onOrdersChanged.Broadcast();
}

// helper function for the receptacle class to toggle the sprites correctly
EItem AWaffleTrialsGameState::getItem(int32 slotIndex, int32 itemIndex) const{
	if (!orders.IsValidIndex(slotIndex))
		return EItem::None;

	const FOrder& order = orders[slotIndex];
	if (!order.active())
		return EItem::None;
	if (!order.items.IsValidIndex(itemIndex))
		return EItem::None;

	const FOrderItem& orderItem = order.items[itemIndex];
	return orderItem.delivered ? EItem::None : orderItem.item;
}

void AWaffleTrialsGameState::setOrder(int32 slotIndex, const FOrder& newOrder){
	if (!HasAuthority()) return;
	if (slotIndex < 0) return;

	if (slotIndex >= orders.Num()){
		orders.SetNum(slotIndex + 1);
	}

	orders[slotIndex] = newOrder;
	onOrdersChanged.Broadcast();
}

void AWaffleTrialsGameState::clearOrder(int32 slotIndex){
	if (!HasAuthority()) return;
	if (!orders.IsValidIndex(slotIndex)) return;

	orders[slotIndex].orderId = -1;
	orders[slotIndex].items.Empty();
	onOrdersChanged.Broadcast();
}

void AWaffleTrialsGameState::loseLife(){
	if (!HasAuthority()) return;
	if (gameOver) return;

	currentLives = FMath::Max(0, currentLives - 1);
	onLivesChanged.Broadcast();

	if (currentLives == 0){
		gameOver = true;
		onGameOver.Broadcast();
	}
}

void AWaffleTrialsGameState::OnRep_Lives(){
	onLivesChanged.Broadcast();
}

void AWaffleTrialsGameState::OnRep_GameOver(){
	onGameOver.Broadcast();
}

float AWaffleTrialsGameState::getTimeRemaining(int32 slotIndex) const{
	if (!orders.IsValidIndex(slotIndex))
		return 0.f;

	const FOrder& order = orders[slotIndex];
	if (!order.active())
		return 0.f;

	const float duration = order.timeLimit - order.startTime;
	if (duration <= 0.f)
		return 0.f;

	const float remaining = order.timeLimit - GetServerWorldTimeSeconds();
	return FMath::Clamp(remaining / duration, 0.f, 1.f);
}