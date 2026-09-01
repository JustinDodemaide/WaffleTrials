// Fill out your copyright notice in the Description page of Project Settings.


#include "DonutCase.h"
#include "WaffleTrialsCharacter.h"

void ADonutCase::Interact(APawn* Interactor) {
	if (!HasAuthority())
		return;

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player)
		return;

	if (player->GetHeldItem() != EItem::None)
		return;

	player->SetHeldItem(EItem::Donut);
	MulticastPlayAnim();
}

void ADonutCase::MulticastPlayAnim_Implementation() {
	Mesh->PlayAnimation(anim, false);
}