// Fill out your copyright notice in the Description page of Project Settings.


#include "CarriedItemSprite.h"
#include "PaperSpriteComponent.h"

// Sets default values
ACarriedItemSprite::ACarriedItemSprite()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	spriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ItemSpriteComponent"));
	RootComponent = spriteComponent;
	spriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACarriedItemSprite::setSprite(UPaperSprite* sprite) {
	if (spriteComponent)
		spriteComponent->SetSprite(sprite);
}

