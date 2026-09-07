// Fill out your copyright notice in the Description page of Project Settings.

#include "Customer.h"
#include "PaperFlipbook.h"

ACustomer::ACustomer() {
	PrimaryActorTick.bCanEverTick = true;

	// cosmetic only, so the server never needs to know about these
	bReplicates = false;

	flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("flipbook"));
	RootComponent = flipbook;
	flipbook->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	flipbook->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	flipbook->SetRelativeScale3D(FVector(2f));
}

void ACustomer::BeginPlay() {
	Super::BeginPlay();

	setState(ECustomerState::Hidden);
}

bool ACustomer::isBusy() const {
	return state != ECustomerState::Hidden;
}

void ACustomer::arrive(const FVector& spawnPos, const FVector& seatPos) {
	fromPos = spawnPos;
	toPos = seatPos;
	walkProgress = 0.f;

	SetActorLocation(spawnPos);
	setState(ECustomerState::WalkingIn);
}

void ACustomer::leave() {
	FVector currentPos = GetActorLocation();
	toPos = fromPos;
	fromPos = currentPos;
	walkProgress = 0.f;
	setState(ECustomerState::WalkingOut);
}

void ACustomer::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	if (state != ECustomerState::WalkingIn && state != ECustomerState::WalkingOut)
		return;

	float distance = FVector::Dist(fromPos, toPos);
	if (distance <= 1.f) {
		walkProgress = 1.f;
	}
	else {
		walkProgress += (walkSpeed * deltaTime) / distance;
		walkProgress = FMath::Clamp(walkProgress, 0.f, 1.f);
	}

	SetActorLocation(FMath::Lerp(fromPos, toPos, walkProgress));

	if (walkProgress < 1.f)
		return;

	if (state == ECustomerState::WalkingIn) {
		setState(ECustomerState::Sitting);
		return;
	}

	setState(ECustomerState::Hidden);
}

void ACustomer::setState(ECustomerState newState) {
	state = newState;

	if (state == ECustomerState::Hidden) {
		flipbook->SetVisibility(false);
		return;
	}

	flipbook->SetVisibility(true);

	if (state == ECustomerState::Sitting) {
		flipbook->SetFlipbook(sitAnim);
		return;
	}

	flipbook->SetFlipbook(walkAnim);
	faceDirection();
}

void ACustomer::faceDirection() {
	// mirror the sprite so they always face the way theyre moving
	float direction = (toPos.X - fromPos.X) >= 0.f ? 1.f : -1.f;

	FVector scale = flipbook->GetRelativeScale3D();
	scale.X = FMath::Abs(scale.X) * direction;
	flipbook->SetRelativeScale3D(scale);
}