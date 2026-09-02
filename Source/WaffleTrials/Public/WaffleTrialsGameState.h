// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ItemType.h"
#include "WaffleTrialsGameState.generated.h"

/**
 * 
 */
USTRUCT()
struct FOrderItem{
	GENERATED_BODY()

	UPROPERTY()
	EItem item = EItem::None;

	UPROPERTY()
	bool delivered = false;
};

USTRUCT()
struct FOrder{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FOrderItem> items;

	UPROPERTY()
	int32 orderId = -1;

	bool active() const { return orderId >= 0; }

	bool fulfilled() const{
		for (const FOrderItem& i : items){
			if (i.item != EItem::None && !i.delivered)
				return false;
		}
		return true;
	}
};

UCLASS()
class WAFFLETRIALS_API AWaffleTrialsGameState : public AGameStateBase
{
	GENERATED_BODY()


public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	DECLARE_MULTICAST_DELEGATE(FOnOrdersChanged);
	FOnOrdersChanged onOrdersChanged;

	bool attemptSubmitItem(int32 slotIndex, EItem Item);
	
	// helper function for the receptacle class to toggle the sprites correctly
	EItem getItem(int32 slotIndex, int32 itemIndex) const;

	// gamemode sets the order slots, not this class
	void setOrder(int32 slotIndex, const FOrder& newOrder);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveOrders)
	TArray<FOrder> orders;

	UFUNCTION()
	void OnRep_ActiveOrders();
};
