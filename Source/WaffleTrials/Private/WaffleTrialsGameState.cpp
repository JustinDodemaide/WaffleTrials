// Fill out your copyright notice in the Description page of Project Settings.


#include "WaffleTrials/WaffleTrialsGameMode.h"
#include "WaffleTrialsGameState.h"
#include "Net/UnrealNetwork.h"

void AWaffleTrialsGameState::BeginPlay()
{

}

void AWaffleTrialsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWaffleTrialsGameState, orders);
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