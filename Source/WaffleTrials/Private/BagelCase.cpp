// Fill out your copyright notice in the Description page of Project Settings.


#include "BagelCase.h"
#include "WaffleTrialsCharacter.h"

void ABagelCase::Interact(APawn* Interactor) {
	if (!HasAuthority())
		return;

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player)
		return;

	if (player->GetHeldItem() != EItem::CookedBEC) return;

	player->SetHeldItem(EItem::PreppedBEC);
	MulticastPlayAnim();
	MulticastPlayInteractSound();
}

void ABagelCase::MulticastPlayAnim_Implementation() {
	Mesh->PlayAnimation(anim, false);
}