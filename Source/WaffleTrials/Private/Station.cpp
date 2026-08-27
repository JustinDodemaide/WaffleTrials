// Fill out your copyright notice in the Description page of Project Settings.

#include "Station.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"

AStation::AStation()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CountText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountText"));
	RootComponent = CountText;
	CountText->SetHorizontalAlignment(EHTA_Center);
	CountText->SetWorldSize(80.f);
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