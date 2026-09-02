// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderReceptacle.h"
#include "WaffleTrialsGameState.h"
#include "WaffleTrialsCharacter.h"
#include "WGameInstance.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "WaffleTrials.h"

AOrderReceptacle::AOrderReceptacle(){
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

	progressBar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("progressBar"));
	progressBar->SetupAttachment(spriteRoot);
}

void AOrderReceptacle::BeginPlay(){
	Super::BeginPlay();
	
	sprites = {sprite1, sprite2, sprite3};

	// instead of making the game state scan every actor,
	// we can make this actor subscribe to the game state's
	// onOrdersChanged call (connecting updateVisuals to it)
	// like godot's observer pattern
	AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>();
	if (!gameState)
		return;
	ordersChangedHandle = gameState->onOrdersChanged.AddUObject(this, &AOrderReceptacle::updateVisuals);

	updateVisuals();
}

void AOrderReceptacle::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>();
	if (!gameState || gameState->isGameOver()){
		progressBar->SetVisibility(false);
		return;
	}

	const float progress = gameState->getTimeRemaining(orderSlotID);
	progressBar->SetVisibility(progress > 0.f);
	progressBar->SetRelativeScale3D(FVector(progress, 0.1f, 0.25f) * 5);
}

void AOrderReceptacle::updateVisuals()
{
	AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>();
	if (!gameState){
		hideSprites();
		return;
	}

	UWGameInstance* gameInstance = Cast<UWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!gameInstance || !gameInstance->ItemDataTable){
		hideSprites();
		return;
	}

	for (int32 i = 0; i < sprites.Num(); ++i){
		const EItem item = gameState->getItem(orderSlotID, i);

		if (item == EItem::None){
			sprites[i]->SetVisibility(false);
			continue;
		}

		const FString itemName = UEnum::GetDisplayValueAsText(item).ToString();
		FItemData* itemInfo = gameInstance->ItemDataTable->FindRow<FItemData>(
			FName(*itemName), TEXT("looking up sprite"));

		if (!itemInfo){
			sprites[i]->SetVisibility(false);
			continue;
		}

		sprites[i]->SetSprite(itemInfo->sprite);
		sprites[i]->SetVisibility(true);
	}
}

void AOrderReceptacle::hideSprites(){
	for (UPaperSpriteComponent* sprite : sprites)
		sprite->SetVisibility(false);
}

void AOrderReceptacle::Interact(APawn* Interactor){
	//UE_LOG(LogWaffleTrials, Warning, TEXT("FFFFFFFFFFFFFFFFFFFFFFFFFFFFF"));
	if (!HasAuthority()) return;

	AWaffleTrialsCharacter* player = Cast<AWaffleTrialsCharacter>(Interactor);
	if (!player)
		return;

	const EItem playerItem = player->GetHeldItem();
	if (playerItem == EItem::None)
		return;

	AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>();
	if (!gameState)
		return;

	//UE_LOG(LogWaffleTrials, Warning, TEXT("HHHHHHHHHHHHHHHHHHHHHHHHHHHH"));
	if (gameState->attemptSubmitItem(orderSlotID, playerItem)) {
		player->SetHeldItem(EItem::None);
		//UE_LOG(LogWaffleTrials, Warning, TEXT("XXXXXXXXXXXXXXXXXXXX"));

	}

	//UE_LOG(LogWaffleTrials, Warning, TEXT("GGGGGGGGGGGGGGGGG"));
}

// need to manually unsubscribe from the "signal" when this object gets deallocated
void AOrderReceptacle::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	AWaffleTrialsGameState* gameState = GetWorld()->GetGameState<AWaffleTrialsGameState>();
	if (!gameState)
		return;
	gameState->onOrdersChanged.Remove(ordersChangedHandle);

	progressBar->SetVisibility(false);

	Super::EndPlay(EndPlayReason);
}
