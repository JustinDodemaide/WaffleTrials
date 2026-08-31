// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "Fridge.generated.h"

/**
 *
 */
UCLASS()
class WAFFLETRIALS_API AFridge : public AStation
{
	GENERATED_BODY()

public:
	virtual void Interact(APawn* Interactor) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> anim;

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAnim();
};
