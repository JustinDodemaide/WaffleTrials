// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderReceptacle.h"

// Sets default values
AOrderReceptacle::AOrderReceptacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	spriteRoot = CreateDefaultSubobject<USceneComponent>(TEXT("spriteRoot"));
	spriteRoot->SetupAttachment(Mesh);
	sprite1 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("sprite1"));
	sprite1->SetupAttachment(spriteRoot);
	sprite2 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("sprite2"));
	sprite2->SetupAttachment(spriteRoot);
	sprite3 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("sprite3"));
	sprite3->SetupAttachment(spriteRoot);
}

// Called when the game starts or when spawned
void AOrderReceptacle::BeginPlay()
{
	Super::BeginPlay();
	
	sprites = {sprite1, sprite2, sprite3};
}

void AOrderReceptacle::Interact(APawn* Interactor) {

}