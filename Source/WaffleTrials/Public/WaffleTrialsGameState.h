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

	// if we record start time, we can use GetServerWorldTimeSeconds - startTime to get a timeRemaining
	// without needing to replicate any other values
	UPROPERTY()
	float startTime = 0.f;

	UPROPERTY()
	float timeLimit = 0.f;

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

	float getTimeRemaining(int32 slotIndex) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveOrders)
	TArray<FOrder> orders;

	UFUNCTION()
	void OnRep_ActiveOrders();


// --- life section start ---
public:
	void loseLife();
	void clearOrder(int32 slotIndex);

	int32 getLives() const {
		return currentLives;
	}
	DECLARE_MULTICAST_DELEGATE(FOnLivesChanged);
	FOnLivesChanged onLivesChanged;

	bool isGameOver() const {
		return gameOver;
	}

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnGameOver onGameOver;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 startingLives = 3;

	// we keep this in state rather than mode so we can display it for each player
	UPROPERTY(ReplicatedUsing = OnRep_Lives)
	int32 currentLives = 3;

	UPROPERTY(ReplicatedUsing = OnRep_GameOver)
	bool gameOver = false;

	UFUNCTION()
	void OnRep_Lives();

	UFUNCTION()
	void OnRep_GameOver();
// --- life section end ---

// --- score section start
public:
	UFUNCTION(BlueprintPure, Category = "Game")
	int32 getScore() const {
		return score;
	}

	DECLARE_MULTICAST_DELEGATE(FOnScoreChanged);
	FOnScoreChanged onScoreChanged;

protected:
	UPROPERTY(EditDefaultsOnly)
	int32 pointsPerOrder = 100;

	UPROPERTY(ReplicatedUsing = OnRep_Score)
	int32 score = 0;

	UFUNCTION()
	void OnRep_Score();
// --- score section end ---
};
