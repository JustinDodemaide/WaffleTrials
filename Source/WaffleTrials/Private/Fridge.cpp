// Fill out your copyright notice in the Description page of Project Settings.


#include "Fridge.h"
#include "WaffleTrialsCharacter.h"

void AFridge::Interact(APawn* Interactor) {
	if (!HasAuthority())
		return;

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player)
		return;

	if (player->GetHeldItem() != EItem::None) return;

	player->SetHeldItem(EItem::RawBEC);
	MulticastPlayAnim();
	MulticastPlayInteractSound();
}

void AFridge::MulticastPlayAnim_Implementation() {
	Mesh->PlayAnimation(anim, false);
}