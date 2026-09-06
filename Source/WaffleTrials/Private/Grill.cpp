// Fill out your copyright notice in the Description page of Project Settings.

#include "Grill.h"
#include "Net/UnrealNetwork.h"
#include "WaffleTrialsCharacter.h"


void AGrill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGrill, state);
}

AGrill::AGrill() {
    bReplicates = true;

    Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

    spriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("spriteComponent"));
    spriteComponent->SetupAttachment(Mesh);
    spriteComponent->SetVisibility(false);
}

void AGrill::BeginPlay() {
    // im dumb and made the anim close -> open and we need it to start open,
    // so play the anim when the game starts
    Super::BeginPlay();

    if (Mesh && anim) {
        Mesh->SetAnimation(anim);
        Mesh->SetPosition(anim->GetPlayLength(), false);
        Mesh->Stop();
    }

    updateVisuals();
}

void AGrill::Interact(APawn* Interactor)
{
    if (!HasAuthority()) return;

    AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
    if (!player) return;

    switch (state)
    {
    case EGrillState::Empty:
        // if player has raw ingredients, start cooking
        if (player->GetHeldItem() != EItem::RawBEC) return;
        player->SetHeldItem(EItem::None);
        state = EGrillState::Cooking;
        updateVisuals();
        GetWorldTimerManager().SetTimer(timer, this, &AGrill::timeout, 5.0f, false);
        break;

    case EGrillState::Done:
        // let them take the prepped BEC, set to empty
        if (player->GetHeldItem() != EItem::None) return;
        GetWorldTimerManager().ClearTimer(timer);
        player->SetHeldItem(EItem::CookedBEC);
        state = EGrillState::Empty;
        updateVisuals();
        break;

    case EGrillState::Burnt:
        // let them take the rubbish, set to empty
        if (player->GetHeldItem() != EItem::None) return;
        player->SetHeldItem(EItem::Rubbish);
        state = EGrillState::Empty;
        updateVisuals();
        break;

    default:
        // if its cooking, do nothing
        break;
    }
}

void AGrill::timeout()
{
    if (state == EGrillState::Cooking)
    {
        state = EGrillState::Done;
        updateVisuals();
        GetWorldTimerManager().SetTimer(timer, this, &AGrill::timeout, 8.0f, false);
    }
    else if (state == EGrillState::Done)
    {
        state = EGrillState::Burnt;
        updateVisuals();
    }
}


void AGrill::updateVisuals() {
    switch (state) {
    case EGrillState::Empty:
       spriteComponent->SetVisibility(false);
        break;
    case EGrillState::Cooking:
        Mesh->SetAnimation(anim);
        Mesh->Play(false);
        spriteComponent->SetSprite(cookingSprite);
        spriteComponent->SetVisibility(true); // just in case
        break;
    case EGrillState::Done:
        spriteComponent->SetSprite(doneSprite);
        spriteComponent->SetVisibility(true);
        break;
    case EGrillState::Burnt:
        spriteComponent->SetSprite(burntSprite);
        // sprite visibility should already be true
        spriteComponent->SetVisibility(true); // but just in case
    }
}

/*
void AGrill::setOpen(bool open) {
    if (!Mesh || !anim)
        return;

    const float Len = anim->GetPlayLength();
    Mesh->SetAnimation(anim);

    Mesh->SetPosition(open ? 0.0f : Len, false);
    Mesh->SetPlayRate(open ? 1.0f : -1.0f);
    Mesh->Play(false);
}
*/

void AGrill::stateChanged() {
    updateVisuals();
}