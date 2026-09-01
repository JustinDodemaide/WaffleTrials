// Fill out your copyright notice in the Description page of Project Settings.


#include "Trash.h"
#include "WaffleTrialsCharacter.h"

void ATrash::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATrash, state);
}

ATrash::ATrash() {
	bReplicates = true;
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
	GetWorldTimerManager().SetTimer(timer, this, &ATrash::timeout, 5.0f, false);
}

void ATrash::timeout() {
	state = ETrashState::Ready;
}

void ATrash::stateChanged() {

}

void ATrash::MulticastPlayAnim_Implementation(){
	Mesh->PlayAnimation(anim, false);
}