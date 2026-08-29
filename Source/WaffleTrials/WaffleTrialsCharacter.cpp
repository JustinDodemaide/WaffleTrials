// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaffleTrialsCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "WaffleTrials.h"

#include "PaperSpriteComponent.h"
#include "PaperSprite.h"

#include "WaffleTrialsPlayerController.h"
#include "InteractorComponent.h"

#include "Kismet/GameplayStatics.h"
#include "WGameInstance.h"

#include "Net/UnrealNetwork.h"

AWaffleTrialsCharacter::AWaffleTrialsCharacter()
{
	Interactor = CreateDefaultSubobject<UInteractorComponent>(TEXT("Interactor"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CarriedItemSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CarriedItemSprite"));
	CarriedItemSprite->SetupAttachment(RootComponent);
	CarriedItemSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarriedItemSprite->SetRelativeLocation(FVector(60.0f, 0.0f, 20.0f));
}

void AWaffleTrialsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWaffleTrialsCharacter, HeldItem);
}

void AWaffleTrialsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWaffleTrialsCharacter::Move);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AWaffleTrialsCharacter::Interact);
	}
	else
	{
		UE_LOG(LogWaffleTrials, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AWaffleTrialsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AWaffleTrialsCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AWaffleTrialsCharacter::Interact(){
	UE_LOG(LogWaffleTrials, Warning, TEXT("Interact pressed"));

	AWaffleTrialsPlayerController* pc = Cast<AWaffleTrialsPlayerController>(GetController());
	if (pc)
		pc->TryInteract();
}

void AWaffleTrialsCharacter::SetHeldItem(EItem item){
	if (!HasAuthority()){
		UE_LOG(LogWaffleTrials, Warning, TEXT("SetHeldItem called on a client; ignoring."));
		return;
	}

	HeldItem = item;

	// OnRep_HeldItem does not fire on the server, so apply it here too.
	UpdateCarriedItemSprite();
}

EItem AWaffleTrialsCharacter::GetHeldItem() const{
	return HeldItem;
}

void AWaffleTrialsCharacter::OnRep_HeldItem(){
	UpdateCarriedItemSprite();
}

void AWaffleTrialsCharacter::UpdateCarriedItemSprite(){
	if (!CarriedItemSprite)
		return;

	if (HeldItem == EItem::None){
		CarriedItemSprite->SetSprite(nullptr);
		return;
	}

	UWGameInstance* GameInstance = Cast<UWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance){
		UE_LOG(LogWaffleTrials, Error, TEXT("no game instance"));
		CarriedItemSprite->SetSprite(nullptr);
		return;
	}

	if (!GameInstance->ItemDataTable) {
		UE_LOG(LogWaffleTrials, Error, TEXT("no data table"));
		CarriedItemSprite->SetSprite(nullptr);
		return;
	}

	const FString ItemName = UEnum::GetDisplayValueAsText(HeldItem).ToString();
	const FName RowName(*ItemName);

	FItemData* ItemInfo = GameInstance->ItemDataTable->FindRow<FItemData>(RowName, TEXT("looking up sprite"));
	if (!ItemInfo){
		UE_LOG(LogWaffleTrials, Error, TEXT("item data not found in table :/"), *ItemName);
		CarriedItemSprite->SetSprite(nullptr);
		return;
	}

	CarriedItemSprite->SetSprite(ItemInfo->sprite);
}