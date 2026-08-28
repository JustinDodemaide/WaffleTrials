// Fill out your copyright notice in the Description page of Project Settings.

#include "Station.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

AStation::AStation()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	CountText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountText"));
	//RootComponent = CountText;
	CountText->SetHorizontalAlignment(EHTA_Center);
	CountText->SetWorldSize(80.f);
	CountText->SetupAttachment(Mesh);
	CountText->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
}

void AStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStation, Count);
}

void AStation::Use()
{
	if (!HasAuthority())
	{
		return;
	}

	Count++;
	CountText->SetText(FText::AsNumber(Count));
}

void AStation::OnRep_Use()
{
	CountText->SetText(FText::AsNumber(Count));
}

void AStation::SetHighlight(bool highlight) {
	Mesh->SetRenderCustomDepth(highlight);

	if (highlight) {
		CountText->SetTextRenderColor(FColor::Green);
	}
	else {
		CountText->SetTextRenderColor(FColor::White);
	}
}