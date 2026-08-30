// Fill out your copyright notice in the Description page of Project Settings.


#include "WaffleMaker.h"
#include "WaffleTrialsCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Components/TextRenderComponent.h"

AWaffleMaker::AWaffleMaker()
{
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	bReplicates = true;
}

void AWaffleMaker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWaffleMaker, State);
}

void AWaffleMaker::Interact(APawn* Interactor) {
	if (!HasAuthority()) {
		return;
	}

	switch (State) {
	case EState::Empty:
		SetState(EState::Cooking);
		GetWorldTimerManager().SetTimer(CookTimerHandle, this, &AWaffleMaker::FinishCooking, CookTime, false);
		break;

	case EState::Cooking:
		// do nothing while its cooking
		break;

	case EState::Done:
		if (TryGiveItem(Interactor, EItem::Waffle)) {
			SetState(EState::Empty);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("WaffleMaker: could not give Waffle to %s"), *GetNameSafe(Interactor));
		}
		break;

	case EState::Burnt:
		if (TryGiveItem(Interactor, EItem::Rubbish)) {
			SetState(EState::Empty);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("WaffleMaker: could not give Rubbish to %s"), *GetNameSafe(Interactor));
		}
		break;
	}
}

void AWaffleMaker::FinishCooking() {
	SetState(EState::Done);
	GetWorldTimerManager().SetTimer(CookTimerHandle, this, &AWaffleMaker::BurnWaffle, BurnTime, false);
}

void AWaffleMaker::BurnWaffle() {
	SetState(EState::Burnt);
}

bool AWaffleMaker::TryGiveItem(APawn* Interactor, EItem Item) {
	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player) {
		return false;
	}

	if (player->GetHeldItem() != EItem::None) {
		return false;
	}

	player->SetHeldItem(Item);
	return true;
}

void AWaffleMaker::SetState(EState NewState) {
	if (State == NewState) {
		return;
	}

	State = NewState;

	if (State == EState::Empty) {
		GetWorldTimerManager().ClearTimer(CookTimerHandle);
	}

	UpdateVisuals();
}

void AWaffleMaker::OnRep_State() {
	UpdateVisuals();
}

void AWaffleMaker::UpdateVisuals() {
	SetOpen(State != EState::Cooking, false);
	RefreshCountdown();
	OnStateChanged(State);
}

void AWaffleMaker::RefreshCountdown()
{
	GetWorldTimerManager().ClearTimer(CountTextTimerHandle);

	float Duration = 0.0f;
	switch (State) {
	case EState::Cooking:
		Duration = CookTime;
		break;
	case EState::Done:
		Duration = BurnTime;
		break;
	default:
		break;
	}

	if (Duration > 0.0f && GetWorld()) {
		CountEndTime = GetWorld()->GetTimeSeconds() + Duration;
		GetWorldTimerManager().SetTimer(CountTextTimerHandle, this, &AWaffleMaker::UpdateCountText, 0.1f, true);
	}

	UpdateCountText();
}

void AWaffleMaker::UpdateCountText()
{
	if (!CountText || !GetWorld()) {
		return;
	}

	FText NewText = FText::GetEmpty();
	FColor NewColor = FColor::White;

	switch (State) {
	case EState::Cooking:
	case EState::Done:
	{
		const float Remaining = FMath::Max(0.0f, CountEndTime - GetWorld()->GetTimeSeconds());
		NewText = FText::FromString(FString::Printf(TEXT("%.1f"), Remaining));
		NewColor = (State == EState::Cooking) ? FColor::White : FColor::Orange;

		if (Remaining <= 0.0f) {
			GetWorldTimerManager().ClearTimer(CountTextTimerHandle);
		}
		break;
	}

	case EState::Burnt:
		NewText = FText::FromString(TEXT("Burnt"));
		NewColor = FColor::Red;
		break;

	default:
		break;
	}

	CountText->SetText(NewText);
	CountText->SetTextRenderColor(NewColor);
}

void AWaffleMaker::SetOpen(bool bOpen, bool bInstant)
{
	if (!Mesh || !CookingAnim) {
		return;
	}

	if (bIsOpen == bOpen && !bInstant) {
		return;
	}

	bIsOpen = bOpen;

	const float Len = CookingAnim->GetPlayLength();
	Mesh->SetAnimation(CookingAnim);

	if (bInstant) {
		Mesh->SetPlayRate(1.0f);
		Mesh->SetPosition(bOpen ? Len : 0.0f, false);
		Mesh->Stop();
		return;
	}

	Mesh->SetPosition(bOpen ? 0.0f : Len, false);
	Mesh->SetPlayRate(bOpen ? 1.0f : -1.0f);
	Mesh->Play(false);
}

void AWaffleMaker::BeginPlay() {
	Super::BeginPlay();

	SetOpen(State != EState::Cooking, true);
	UpdateCountText();
}