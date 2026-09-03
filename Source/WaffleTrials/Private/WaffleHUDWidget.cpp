// Fill out your copyright notice in the Description page of Project Settings.


#include "WaffleHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "WaffleTrialsGameState.h"

void UWaffleHUDWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>()) {
		scoreHandle = gameState->onScoreChanged.AddUObject(this, &UWaffleHUDWidget::updateScore);
		livesHandle = gameState->onLivesChanged.AddUObject(this, &UWaffleHUDWidget::updateLives);
	}

	updateScore();
	updateLives();
}

void UWaffleHUDWidget::updateScore() {
	AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>();
	if (!gameState || !scoreText)
		return;

	scoreText->SetText(FText::AsNumber(gameState->getScore()));
}

void UWaffleHUDWidget::updateLives() {
	AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>();
	if (!gameState || !livesContainer)
		return;

	livesContainer->ClearChildren();

	for (int32 i = 0; i < gameState->getLives(); ++i) {
		UImage* heart = NewObject<UImage>(this);
		heart->SetBrushFromTexture(lifeIcon);
		heart->SetDesiredSizeOverride(heartSize);

		if (UHorizontalBoxSlot* boxSlot = livesContainer->AddChildToHorizontalBox(heart)) {
			boxSlot->SetPadding(FMargin(4.f, 0.f));
		}
	}
}

void UWaffleHUDWidget::NativeDestruct() {
	if (AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>()) {
		gameState->onScoreChanged.Remove(scoreHandle);
		gameState->onLivesChanged.Remove(livesHandle);
	}

	Super::NativeDestruct();
}