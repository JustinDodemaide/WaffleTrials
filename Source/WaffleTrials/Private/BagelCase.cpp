// Fill out your copyright notice in the Description page of Project Settings.


#include "BagelCase.h"
#include "WaffleTrialsCharacter.h"

void ABagelCase::Interact(APawn* Interactor) {
	// Super::Interact(Interactor);

	if (!HasAuthority()) {
		return;
	}

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (player)
	{
		if (player->GetHeldItem() != EItem::None)
			return;

		player->SetHeldItem(EItem::Bagel);
	}
}