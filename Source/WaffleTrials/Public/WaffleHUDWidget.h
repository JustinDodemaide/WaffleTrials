// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaffleHUDWidget.generated.h"

UCLASS()
class WAFFLETRIALS_API UWaffleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> scoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> livesContainer;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture2D> lifeIcon;

	UPROPERTY(EditDefaultsOnly)
	FVector2D heartSize = FVector2D(48.f, 48.f);

	void updateScore();
	void updateLives();

	FDelegateHandle scoreHandle;
	FDelegateHandle livesHandle;
};