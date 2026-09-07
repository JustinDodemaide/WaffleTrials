
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "Interactable.h"
#include "Station.generated.h"

class UTextRenderComponent;

UCLASS()
class WAFFLETRIALS_API AStation : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AStation();

	virtual void Interact(APawn* Interactor) override;
	virtual void Targeted(bool bTargeted) override;

protected:
	virtual void BeginPlay() override;

	//UPROPERTY(ReplicatedUsing = OnRep_Count)
	//int32 Count = 0;

	//UFUNCTION()
	//void OnRep_Count();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	//UPROPERTY(VisibleAnywhere, Category = "Components")
	//TObjectPtr<UTextRenderComponent> CountText;

	// the outline shader never looked right, so we point a big arrow at it instead
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPaperSpriteComponent> indicator;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight")
	TObjectPtr<class UPaperSprite> arrowImage;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight")
	float arrowHeight = 250.f;

	void SetHighlight(bool highlighted);

	//void UpdateCount();
};