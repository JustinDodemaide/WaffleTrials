// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ItemType.h"
#include "WaffleTrialsCharacter.generated.h"

class UInputAction;
struct FInputActionValue;

class UPaperSpriteComponent;
class UPaperSprite;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AWaffleTrialsCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

public:

	/** Constructor */
	AWaffleTrialsCharacter();	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void Interact();

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UInteractorComponent> Interactor;

	// Item carrying
protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPaperSpriteComponent> CarriedItemSprite;

	UPROPERTY(ReplicatedUsing = OnRep_HeldItem)
	EItem HeldItem = EItem::None;

	UFUNCTION()
	void OnRep_HeldItem();

	void UpdateCarriedItemSprite();

public:

	void SetHeldItem(EItem item);
	EItem GetHeldItem() const;
};