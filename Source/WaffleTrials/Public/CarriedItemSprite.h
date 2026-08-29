// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarriedItemSprite.generated.h"

class UPaperSpriteComponent;

UCLASS()
class WAFFLETRIALS_API ACarriedItemSprite : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACarriedItemSprite();

	TObjectPtr<UPaperSpriteComponent> spriteComponent;

	void setSprite(class UPaperSprite* sprite);

};
