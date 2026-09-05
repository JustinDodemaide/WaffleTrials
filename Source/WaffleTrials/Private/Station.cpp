// Fill out your copyright notice in the Description page of Project Settings.

#include "Station.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

AStation::AStation(){
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	/*
	CountText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountText"));
	CountText->SetHorizontalAlignment(EHTA_Center);
	CountText->SetWorldSize(80.f);
	CountText->SetupAttachment(Mesh);
	CountText->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	*/

	Mesh->SetCustomDepthStencilValue(1);
}

void AStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStation, Count);
}

void AStation::Interact(APawn* Interactor){
	if (!HasAuthority())
		return;

	//Count++;
	//UpdateCount();
}

void AStation::OnRep_Count() {
	UpdateCount();
}

void AStation::UpdateCount() {
	CountText->SetText(FText::AsNumber(Count));
}

void AStation::Targeted(bool targeted) {
	SetHighlight(targeted);
}

void AStation::SetHighlight(bool highlight) {
	Mesh->SetRenderCustomDepth(highlight);

	if (highlight)
		CountText->SetTextRenderColor(FColor::Green);
	else
		CountText->SetTextRenderColor(FColor::White);
}