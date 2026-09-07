// Fill out your copyright notice in the Description page of Project Settings.

#include "Station.h"
#include "Components/TextRenderComponent.h"
#include "PaperSprite.h"
#include "Net/UnrealNetwork.h"

AStation::AStation()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	//CountText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountText"));
	//CountText->SetHorizontalAlignment(EHTA_Center);
	//CountText->SetWorldSize(80.f);
	//CountText->SetupAttachment(Mesh);
	//CountText->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

	indicator = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("indicator"));
	indicator->SetupAttachment(Mesh);
	indicator->SetRelativeLocation(FVector(0.f, 0.f, arrowHeight));
	indicator->SetVisibility(false);
	indicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	indicator->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	// i simply dont want to have to set the image for every station
	static ConstructorHelpers::FObjectFinder<UPaperSprite> arrowFinder(
		TEXT("/Script/Engine.Texture2D'/Game/meshes/arrow.arrow'"));
	if (arrowFinder.Succeeded())
		indicator->SetSprite(arrowFinder.Object);
}

void AStation::BeginPlay()
{
	Super::BeginPlay();

	if (!indicator)
		return;
	if (!arrowImage)
		return;

	indicator->SetSprite(arrowImage);
}

void AStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AStation, Count);
}

void AStation::Interact(APawn* Interactor)
{
	if (!HasAuthority())
	{
		return;
	}

	//Count++;
	//UpdateCount();
}

//void AStation::OnRep_Count() {
//	UpdateCount();
//}

//void AStation::UpdateCount() {
//	CountText->SetText(FText::AsNumber(Count));
//}

void AStation::Targeted(bool targeted) {
	SetHighlight(targeted);
}

void AStation::SetHighlight(bool highlight) {
	if (!indicator)
		return;

	indicator->SetVisibility(highlight);
}