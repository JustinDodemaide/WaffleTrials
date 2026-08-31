// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "PaperSpriteComponent.h"
#include "CoffeeMachine.generated.h"

UENUM(BlueprintType)
enum class ECoffeeMachineState : uint8
{
	Ready,
	NotReady
};

UCLASS()
class WAFFLETRIALS_API ACoffeeMachine : public AStation
{
	GENERATED_BODY()
	
public:
	ACoffeeMachine();
	virtual void Interact(APawn* Interactor) override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> anim;

	FTimerHandle timer;
	void timeout();

	ECoffeeMachineState state;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> spriteComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> sprite;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> readySound;

	void UpdateVisuals();
};
