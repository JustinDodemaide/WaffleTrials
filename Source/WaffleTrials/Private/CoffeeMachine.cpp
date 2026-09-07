// Fill out your copyright notice in the Description page of Project Settings.


#include "CoffeeMachine.h"
#include "Net/UnrealNetwork.h"
#include "WaffleTrialsCharacter.h"

// so theres a chance this object is either the authority
// or its a proxy
// but the controller is always going to call interact through the authority bc of Server_Interact
// how can you tell? HasAuthority()

void ACoffeeMachine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACoffeeMachine, state);
}

ACoffeeMachine::ACoffeeMachine(){
	bReplicates = true;

	spriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("spriteComponent"));
	spriteComponent->SetupAttachment(Mesh);
}

// the proxy never actually calls this. all calls to this
// come from the authority's machine
void ACoffeeMachine::Interact(APawn* Interactor) {
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
	MulticastPlayInteractSound();

	state = ECoffeeMachineState::NotReady;
	GetWorldTimerManager().SetTimer(
		timer,
		this,
		&ACoffeeMachine::timeout,
		5.0f,
		false
	);
}

void ACoffeeMachine::timeout() {
	state = ECoffeeMachineState::Ready;
}

void ACoffeeMachine::updateVisuals() {
	if (state == ECoffeeMachineState::NotReady) {
		spriteComponent->SetVisibility(false);
	}

	if (state == ECoffeeMachineState::Ready) {
		spriteComponent->SetVisibility(true);
		Mesh->PlayAnimation(anim, false);
	}
}

void ACoffeeMachine::stateChanged() {
	updateVisuals();
}