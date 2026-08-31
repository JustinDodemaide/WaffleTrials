// Fill out your copyright notice in the Description page of Project Settings.

#include "WaffleMaker.h"
#include "Net/UnrealNetwork.h"
#include "WaffleTrialsCharacter.h"


void AWaffleMaker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWaffleMaker, state);
}

AWaffleMaker::AWaffleMaker() {
	bReplicates = true;

    Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

    spriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("spriteComponent"));
    spriteComponent->SetupAttachment(Mesh);
    spriteComponent->SetVisibility(false);
}

void AWaffleMaker::BeginPlay(){
    // im dumb and made the anim close -> open and we need it to start open,
    // so play the anim when the game starts
    Super::BeginPlay();

    if (Mesh && anim){
        Mesh->SetAnimation(anim);
        Mesh->SetPosition(anim->GetPlayLength(), false);
        Mesh->Stop();
    }

    updateVisuals();
}

void AWaffleMaker::Interact(APawn* Interactor)
{
    if (!HasAuthority()) return;

    AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
    if (!player) return;

    switch (state)
    {
    case EWaffleMakerState::Empty:
        // close lid, start cooking
        state = EWaffleMakerState::Cooking;
        updateVisuals();
        GetWorldTimerManager().SetTimer(timer, this, &AWaffleMaker::timeout, 5.0f, false);
        break;

    case EWaffleMakerState::Done:
        // let them take the waffle, set to empty
        if (player->GetHeldItem() != EItem::None) return;
        GetWorldTimerManager().ClearTimer(timer);
        player->SetHeldItem(EItem::Waffle);
        state = EWaffleMakerState::Empty;
        updateVisuals();
        break;

    case EWaffleMakerState::Burnt:
        // let them take the rubbish, set to empty
        if (player->GetHeldItem() != EItem::None) return;
        player->SetHeldItem(EItem::Rubbish);
        state = EWaffleMakerState::Empty;
        updateVisuals();
        break;

    default:
        // if its cooking, do nothing
        break;
    }
}

void AWaffleMaker::timeout()
{
    if (state == EWaffleMakerState::Cooking)
    {
        state = EWaffleMakerState::Done;
        updateVisuals();
        GetWorldTimerManager().SetTimer(timer, this, &AWaffleMaker::timeout, 5.0f, false);
    }
    else if (state == EWaffleMakerState::Done)
    {
        state = EWaffleMakerState::Burnt;
        updateVisuals();
    }
}


void AWaffleMaker::updateVisuals() {
    switch (state) {
    case EWaffleMakerState::Empty:
        spriteComponent->SetVisibility(false);
        break;
    case EWaffleMakerState::Cooking:
        setOpen(false);
        spriteComponent->SetVisibility(false); // just in case
        break;
    case EWaffleMakerState::Done:
        setOpen(true);
        spriteComponent->SetSprite(doneSprite);
        spriteComponent->SetVisibility(true);
        break;
    case EWaffleMakerState::Burnt:
        spriteComponent->SetSprite(burntSprite);
        // sprite visibility should already be true
        spriteComponent->SetVisibility(true); // but just in case
    }
}
        
void AWaffleMaker::setOpen(bool open) {
    if (!Mesh || !anim)
        return;

    const float Len = anim->GetPlayLength();
    Mesh->SetAnimation(anim);

    Mesh->SetPosition(open ? 0.0f : Len, false);
    Mesh->SetPlayRate(open ? 1.0f : -1.0f);
    Mesh->Play(false);
}

void AWaffleMaker::stateChanged() {
    updateVisuals();
}