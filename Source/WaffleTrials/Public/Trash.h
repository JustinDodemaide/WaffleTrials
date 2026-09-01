// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "Trash.generated.h"

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
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void Interact(APawn* Interactor) override;

	ATrash();

protected:
	FTimerHandle timer;
	void timeout();

	UPROPERTY(ReplicatedUsing = stateChanged)
	ETrashState state = ETrashState::Ready;

	UFUNCTION()
	void stateChanged();

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> anim;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAnim();
};
