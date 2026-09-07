// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "Customer.generated.h"

UENUM()
enum class ECustomerState : uint8 {
	Hidden,
	WalkingIn,
	Sitting,
	WalkingOut
};

UCLASS()
class WAFFLETRIALS_API ACustomer : public AActor
{
	GENERATED_BODY()

public:
	ACustomer();

	virtual void Tick(float deltaTime) override;

	// the receptacle owns the markers
	void arrive(const FVector& spawnPos, const FVector& seatPos);
	void leave();

	bool isBusy() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPaperFlipbookComponent> flipbook;

	UPROPERTY(EditDefaultsOnly, Category = "Customer")
	TObjectPtr<class UPaperFlipbook> walkAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Customer")
	TObjectPtr<class UPaperFlipbook> sitAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Customer")
	float walkSpeed = 200.f;

	ECustomerState state = ECustomerState::Hidden;

	FVector fromPos = FVector::ZeroVector;
	FVector toPos = FVector::ZeroVector;

	float walkProgress = 0.f;

	void setState(ECustomerState newState);
	void faceDirection();
};