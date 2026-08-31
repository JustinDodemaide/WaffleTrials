// Fill out your copyright notice in the Description page of Project Settings.


#include "Trash.h"
#include "WaffleTrialsCharacter.h"

void ATrash::Interact(APawn* Interactor) {
	if (!HasAuthority())
		return;

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player)
		return;
	
	player->SetHeldItem(EItem::None);
	MulticastPlayAnim();
}

void ATrash::MulticastPlayAnim_Implementation(){
	Mesh->PlayAnimation(anim, false);
}