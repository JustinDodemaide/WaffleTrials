#include "InteractorComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"

UInteractorComponent::UInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		SearchTimer, this, &UInteractorComponent::Update, 0.1f, true);
}

void UInteractorComponent::Update()
{
	const FVector Origin = GetOwner()->GetActorLocation();

	TArray<AActor*> Overlaps;
	UKismetSystemLibrary::SphereOverlapActors(
		this, Origin, SearchRadius,
		{ UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		  UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
		nullptr, { GetOwner() }, Overlaps);

	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AActor* Actor : Overlaps)
	{
		if (!Actor->Implements<UInteractable>())
		{
			continue;
		}

		float DistSq = FVector::DistSquared(Origin, Actor->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Actor;
		}
	}

	if (Best == currentInteractable.GetObject())
	{
		return;
	}

	if (currentInteractable)
	{
		currentInteractable->Targeted(false);
	}

	currentInteractable = Best;

	if (currentInteractable)
	{
		currentInteractable->Targeted(true);
	}
}