// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "Interactable.h"
#include "OrderReceptacle.generated.h"


UCLASS()
class WAFFLETRIALS_API AOrderReceptacle : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AOrderReceptacle();

	virtual void Interact(APawn* Interactor) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	TArray<UPaperSpriteComponent*> sprites;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> spriteRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> sprite1;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> sprite2;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperSpriteComponent> sprite3;

	UPROPERTY(EditAnywhere, Category = "Order")
	int32 orderSlotID = 0;

	FDelegateHandle ordersChangedHandle;

	void updateVisuals();
	void hideSprites();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> progressBar;
};
