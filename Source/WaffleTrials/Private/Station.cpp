// Fill out your copyright notice in the Description page of Project Settings.

#include "Station.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AStation::AStation() {
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

	outlineMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("outlineMesh"));
	outlineMesh->SetupAttachment(Mesh);
	outlineMesh->SetRelativeScale3D(FVector(outlineScale));
	outlineMesh->SetVisibility(false);
	outlineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	outlineMesh->SetCastShadow(false);

	Mesh->SetCustomDepthStencilValue(1);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> outlineFinder(
		TEXT("Material'/Game/meshes/highlight.highlight'"));

	if (outlineFinder.Succeeded()) {
		outlineMaterial = outlineFinder.Object;
	}
}

void AStation::BeginPlay() {
	Super::BeginPlay();

	if (!outlineMesh || !Mesh)
		return;

	outlineMesh->SetSkeletalMesh(Mesh->GetSkeletalMeshAsset());
	outlineMesh->SetLeaderPoseComponent(Mesh);

	if (!outlineMaterial)
		return;

	int32 count = Mesh->GetNumMaterials();
	for (int32 i = 0; i < count; i++)
		outlineMesh->SetMaterial(i, outlineMaterial);
}

void AStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(AStation, Count);
}

void AStation::Interact(APawn* Interactor) {
	if (!HasAuthority())
		return;

	MulticastPlayInteractSound();

	//Count++;
	//UpdateCount();
}

//void AStation::OnRep_Count() {
	//UpdateCount();
//}

//void AStation::UpdateCount() {
//	CountText->SetText(FText::AsNumber(Count));
//}

void AStation::Targeted(bool targeted) {
	SetHighlight(targeted);
}

void AStation::SetHighlight(bool highlight) {
	if (outlineMesh)
		outlineMesh->SetVisibility(highlight);

	//if (highlight)
	//	CountText->SetTextRenderColor(FColor::Green);
	//else
	//	CountText->SetTextRenderColor(FColor::White);
}

void AStation::MulticastPlayInteractSound_Implementation() {
	if (!interactSound)
		return;

	UGameplayStatics::PlaySoundAtLocation(this, interactSound, GetActorLocation());
}