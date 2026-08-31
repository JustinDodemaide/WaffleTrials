// Fill out your copyright notice in the Description page of Project Settings.


#include "CoffeeMachine.h"
#include "WaffleTrialsCharacter.h"

ACoffeeMachine::ACoffeeMachine(){
	spriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("spriteComponent"));
	spriteComponent->SetupAttachment(Mesh);
}

void ACoffeeMachine::Interact(APawn* Interactor) {
	// Super::Interact(Interactor);

	if (!HasAuthority())
		return;

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player)
		return;

	if (state == ECoffeeMachineState::NotReady)
		return;

	if (player->GetHeldItem() != EItem::None)
		return;

	player->SetHeldItem(EItem::Coffee);
	spriteComponent->SetVisibility(false);
	state = ECoffeeMachineState::NotReady;
	GetWorldTimerManager().SetTimer(
		timer,
		this,
		&ACoffeeMachine::timeout,
		1.0f,
		false
	);
}

void ACoffeeMachine::timeout() {
	Mesh->PlayAnimation(anim, false);
	spriteComponent->SetVisibility(true);
	state = ECoffeeMachineState::Ready;
}

void ACoffeeMachine::UpdateVisuals() {
	spriteComponent->SetVisibility(state == ECoffeeMachineState::Ready);
}