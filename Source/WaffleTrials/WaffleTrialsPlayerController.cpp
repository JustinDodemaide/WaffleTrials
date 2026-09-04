// Copyright Epic Games, Inc. All Rights Reserved.


#include "WaffleTrialsPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "WaffleTrials.h"
#include "Widgets/Input/SVirtualJoystick.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

#include "EnhancedInputComponent.h"

#include "InteractorComponent.h"
#include "Interactable.h"

AWaffleTrialsPlayerController::AWaffleTrialsPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void AWaffleTrialsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	setCamera();

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AWaffleTrialsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AWaffleTrialsPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AWaffleTrialsPlayerController::attemptInteract() {
	APawn* pawn = GetPawn();
	if (!pawn)
		return;

	UInteractorComponent* interactor = pawn->FindComponentByClass<UInteractorComponent>();
	if (!interactor)
		return;

	AActor* target = Cast<AActor>(interactor->GetCurrentInteractable().GetObject());
	if (!target)
		return;

	Server_Interact(target);
}

void AWaffleTrialsPlayerController::Server_Interact_Implementation(AActor* target) {
	APawn* pawn = GetPawn();
	if (!target)
		return;
	if (!pawn)
		return;
	if (!target->Implements<UInteractable>())
		return;

	if (FVector::DistSquared(pawn->GetActorLocation(), target->GetActorLocation()) > FMath::Square(300.f))
		return;

	Cast<IInteractable>(target)->Interact(pawn);
}

/*
void AWaffleTrialsPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	setCamera();

	// we have a race condition where BeginPlay is running before the LocalPlayer is set,
	// so its bailing before the hud is assigned
	if (!hudWidget && hudWidgetClass)
	{
		hudWidget = CreateWidget<UWaffleHUDWidget>(this, hudWidgetClass);
		if (hudWidget)
			hudWidget->AddToViewport();
	}
}
*/

void AWaffleTrialsPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	setCamera();

	UE_LOG(LogWaffleTrials, Warning, TEXT("AcknowledgePossession: class %s, existing %s"),
		*GetNameSafe(hudWidgetClass), *GetNameSafe(hudWidget));

	if (!hudWidget && hudWidgetClass)
	{
		hudWidget = CreateWidget<UWaffleHUDWidget>(this, hudWidgetClass);
		UE_LOG(LogWaffleTrials, Warning, TEXT("created hud: %s"), *GetNameSafe(hudWidget));
		if (hudWidget)
			hudWidget->AddToViewport();
	}
}

void AWaffleTrialsPlayerController::setCamera() {
	TArray<AActor*> cameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), fixedCameraTag,
		cameras);
	if (cameras.Num() > 0) {
		SetViewTarget(cameras[0]);
		SetControlRotation(cameras[0]->GetActorRotation());
	}
}