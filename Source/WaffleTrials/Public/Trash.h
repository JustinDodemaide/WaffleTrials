// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "Trash.generated.h"

class UBillboardComponent;

/**
 *
 */

UENUM(BlueprintType)
enum class ETrashState : uint8 {
	Ready,
	CoolingDown
};

UCLASS()
class WAFFLETRIALS_API ATrash : public AStation
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Interact(APawn* Interactor) override;

	ATrash();

protected:
	virtual void BeginPlay() override;

	FTimerHandle timer;
	void timeout();

	UPROPERTY(ReplicatedUsing = stateChanged)
	ETrashState state = ETrashState::Ready;

	UFUNCTION()
	void stateChanged();

	void UpdateSprite();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trash")
	TObjectPtr<UBillboardComponent> cooldownSprite;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> anim;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAnim();
};