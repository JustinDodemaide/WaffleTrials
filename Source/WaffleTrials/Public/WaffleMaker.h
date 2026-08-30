// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "ItemType.h"
#include "WaffleMaker.generated.h"

UENUM(BlueprintType)
enum class EState : uint8 {
	Empty	UMETA(DisplayName = "Empty"),
	Cooking	UMETA(DisplayName = "Cooking"),
	Done	UMETA(DisplayName = "Done"),
	Burnt	UMETA(DisplayName = "Burnt")
};

UCLASS()
class WAFFLETRIALS_API AWaffleMaker : public AStation
{
	GENERATED_BODY()

public:
	AWaffleMaker();

	virtual void Interact(APawn* Interactor) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Waffle Maker")
	EState GetState() const { return State; }

protected:
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimSequence> CookingAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Waffle Maker")
	float CookTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Waffle Maker")
	float BurnTime = 3.0f;

	UPROPERTY(ReplicatedUsing = OnRep_State, VisibleInstanceOnly, BlueprintReadOnly, Category = "Waffle Maker")
	EState State = EState::Empty;

	UFUNCTION()
	void OnRep_State();

	void SetState(EState NewState);

	void UpdateVisuals();

	UFUNCTION(BlueprintImplementableEvent, Category = "Waffle Maker")
	void OnStateChanged(EState NewState);

	bool TryGiveItem(APawn* Interactor, EItem Item);

	void FinishCooking();
	void BurnWaffle();

	FTimerHandle CookTimerHandle;

	void SetOpen(bool bOpen, bool bInstant = false);

	virtual void BeginPlay() override;

	bool bIsOpen = false;

	void RefreshCountdown();
	void UpdateCountText();

	FTimerHandle CountTextTimerHandle;
	float CountEndTime = 0.0f;
};