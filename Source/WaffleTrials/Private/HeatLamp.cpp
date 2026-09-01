// Fill out your copyright notice in the Description page of Project Settings.

#include "HeatLamp.h"
#include "Net/UnrealNetwork.h"
#include "WaffleTrials.h"
#include "WaffleTrialsCharacter.h"
#include "WGameInstance.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"


void AHeatLamp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHeatLamp, HeldItem);
}

AHeatLamp::AHeatLamp() {
	bReplicates = true;

	spriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("spriteComponent"));
	spriteComponent->SetupAttachment(Mesh);
	spriteComponent->SetVisibility(false);
}

void AHeatLamp::BeginPlay() {
	Super::BeginPlay();

	// covers players joining while something is already on the lamp
	updateVisuals();
}

void AHeatLamp::Interact(APawn* Interactor) {
	if (!HasAuthority()) return;

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player) return;

	const EItem playerItem = player->GetHeldItem();

	if (HeldItem == EItem::None) {
		// nothing on the lamp, so take whatever theyre carrying
		if (playerItem == EItem::None) return;

		HeldItem = playerItem;
		player->SetHeldItem(EItem::None);
	}
	else {
		// something on the lamp, only give it to empty hands
		if (playerItem != EItem::None) return;

		player->SetHeldItem(HeldItem);
		HeldItem = EItem::None;
	}

	updateVisuals();
}

void AHeatLamp::stateChanged() {
	updateVisuals();
}

void AHeatLamp::updateVisuals() {
	if (HeldItem == EItem::None) {
		spriteComponent->SetVisibility(false);
		return;
	}

	UWGameInstance* GameInstance = Cast<UWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance) {
		UE_LOG(LogWaffleTrials, Error, TEXT("no game instance"));
		spriteComponent->SetVisibility(false);
		return;
	}
	if (!GameInstance->ItemDataTable) {
		UE_LOG(LogWaffleTrials, Error, TEXT("no data table"));
		spriteComponent->SetVisibility(false);
		return;
	}

	const FString ItemName = UEnum::GetDisplayValueAsText(HeldItem).ToString();
	const FName RowName(*ItemName);
	FItemData* ItemInfo = GameInstance->ItemDataTable->FindRow<FItemData>(RowName, TEXT("looking up sprite"));
	if (!ItemInfo) {
		UE_LOG(LogWaffleTrials, Error, TEXT("item data not found in table :/ %s"), *ItemName);
		spriteComponent->SetVisibility(false);
		return;
	}

	spriteComponent->SetSprite(ItemInfo->sprite);
	spriteComponent->SetVisibility(true);
}