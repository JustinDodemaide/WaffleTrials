// Fill out your copyright notice in the Description page of Project Settings.


#include "Trash.h"
#include "WaffleTrialsCharacter.h"
#include "Components/BillboardComponent.h"
#include "Net/UnrealNetwork.h"

void ATrash::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATrash, state);
}

ATrash::ATrash() {
	bReplicates = true;

	cooldownSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("CooldownSprite"));
	if (RootComponent)
		cooldownSprite->SetupAttachment(RootComponent);
	cooldownSprite->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	cooldownSprite->SetHiddenInGame(true);
	cooldownSprite->bIsScreenSizeScaled = true;
}

void ATrash::BeginPlay() {
	Super::BeginPlay();

	// Covers players who join while the bin is already cooling down.
	UpdateSprite();
}

void ATrash::Interact(APawn* Interactor) {
	if (!HasAuthority())
		return;

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player)
		return;

	if (state == ETrashState::CoolingDown)
		return;

	player->SetHeldItem(EItem::None);
	MulticastPlayAnim();

	state = ETrashState::CoolingDown;
	UpdateSprite(); // OnRep doesn't fire on the authority, so do it by hand
	GetWorldTimerManager().SetTimer(timer, this, &ATrash::timeout, 5.0f, false);
}

void ATrash::timeout() {
	state = ETrashState::Ready;
	UpdateSprite();
}

void ATrash::stateChanged() {
	UpdateSprite();
}

void ATrash::UpdateSprite() {
	if (cooldownSprite)
		cooldownSprite->SetHiddenInGame(state != ETrashState::CoolingDown);
}

void ATrash::MulticastPlayAnim_Implementation() {
	Mesh->PlayAnimation(anim, false);
}