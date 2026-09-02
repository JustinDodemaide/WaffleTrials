// Fill out your copyright notice in the Description page of Project Settings.


#include "WaffleTrialsGameState.h"
#include "Net/UnrealNetwork.h"

void AWaffleTrialsGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	orders.SetNum(4);

	FOrder& testOrder = orders[0];
	testOrder.orderId = 0;
	testOrder.items.SetNum(3);
	testOrder.items[0].item = EItem::Donut;
	testOrder.items[1].item = EItem::Coffee;
	testOrder.items[2].item = EItem::Waffle;

	onOrdersChanged.Broadcast();
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

		if (Order.fulfilled()){
			// score, sound, clear the slot
			Order.orderId = -1;
			Order.items.Empty();
		}

		OrderItem.delivered = true;
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