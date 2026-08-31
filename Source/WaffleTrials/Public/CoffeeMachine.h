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
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	ACoffeeMachine();
	virtual void Interact(APawn* Interactor) override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> anim;

	FTimerHandle timer;
	void timeout();
	
	// I want to know when the state variable changes so I'm going to change it to replicated, which handles it on its own 
	UPROPERTY(ReplicatedUsing = stateChanged)
	ECoffeeMachineState state = ECoffeeMachineState::Ready;

	// this is the listener for when the server says that the state var was replicated
	// so anything that needs to happen when the state changes we gotta put in here
	UFUNCTION()
	void stateChanged();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> spriteComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> sprite;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> readySound;

	void updateVisuals();
};
