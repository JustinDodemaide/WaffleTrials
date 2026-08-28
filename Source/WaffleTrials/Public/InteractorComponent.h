// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactable.h"
#include "InteractorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAFFLETRIALS_API UInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractorComponent();

	TScriptInterface<IInteractable> GetCurrentInteractable() const { return currentInteractable; }

	UPROPERTY(EditAnywhere, Category="Interaction")
	float SearchRadius = 200.f;

protected:
	virtual void BeginPlay() override;

	void Update();

	UPROPERTY(Transient)
	TScriptInterface<IInteractable> currentInteractable;

	FTimerHandle SearchTimer;
};
