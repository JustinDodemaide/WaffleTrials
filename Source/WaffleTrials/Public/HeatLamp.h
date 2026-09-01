// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "PaperSpriteComponent.h"
#include "WaffleTrialsCharacter.h" // for EItem
#include "HeatLamp.generated.h"

UCLASS()
class WAFFLETRIALS_API AHeatLamp : public AStation
{
	GENERATED_BODY()

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void BeginPlay() override;

	AHeatLamp();

	UFUNCTION(BlueprintPure, Category = "HeatLamp")
	EItem GetHeldItem() const { return HeldItem; }

	UFUNCTION(BlueprintPure, Category = "HeatLamp")
	bool IsEmpty() const { return HeldItem == EItem::None; }

protected:
	UPROPERTY(ReplicatedUsing = stateChanged)
	EItem HeldItem = EItem::None;

	UFUNCTION()
	void stateChanged();

	void updateVisuals();

	virtual void Interact(APawn* Interactor) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> spriteComponent;
};