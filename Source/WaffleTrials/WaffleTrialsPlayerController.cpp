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

#include "Station.h"
#include "EngineUtils.h"

#include "EnhancedInputComponent.h"

void AWaffleTrialsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		TArray<AActor*> cameras;
		UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), KitchenCameraTag,
			cameras);
		if (cameras.Num() > 0) {
			SetViewTarget(cameras[0]);
			SetControlRotation(cameras[0]->GetActorRotation());
		}
	}
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

void AWaffleTrialsPlayerController::UseStation() {
	for (TActorIterator<AStation> It(GetWorld()); It; ++It) {
		Server_UseStation(*It);
		return;
	}
}

void AWaffleTrialsPlayerController::Server_UseStation_Implementation(AStation* Station) {
	if (Station) {
		Station->Use();
	}
}