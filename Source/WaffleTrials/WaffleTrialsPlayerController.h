// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WaffleHUDWidget.h"
#include "WaffleTrialsPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;


/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AWaffleTrialsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWaffleTrialsPlayerController();

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FName fixedCameraTag = FName("KitchenCamera");

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> UseAction;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWaffleHUDWidget> hudWidgetClass;

	UPROPERTY()
	TObjectPtr<UWaffleHUDWidget> hudWidget;

	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	bool ShouldUseTouchControls() const;

	virtual void AcknowledgePossession(APawn* P) override;

	void setCamera();

public:
	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* target);

	void attemptInteract();
};
