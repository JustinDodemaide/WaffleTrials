// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "PaperSpriteComponent.h"
#include "WaffleMaker.generated.h"

UENUM(BlueprintType)
enum class EWaffleMakerState : uint8{
	Empty,
	Cooking,
	Done,
	Burnt
};

UCLASS()
class WAFFLETRIALS_API AWaffleMaker : public AStation
{
	GENERATED_BODY()

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void BeginPlay() override;

	AWaffleMaker();

protected:
	FTimerHandle timer;
	void timeout();

	UPROPERTY(ReplicatedUsing = stateChanged)
	EWaffleMakerState state = EWaffleMakerState::Empty;

	UFUNCTION()
	void stateChanged();

	void updateVisuals();

	virtual void Interact(APawn* Interactor) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> spriteComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> burntSprite;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> doneSprite;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> anim;

	//bool open;
	void setOpen(bool open);
};