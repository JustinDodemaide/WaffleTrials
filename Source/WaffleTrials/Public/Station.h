
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> outlineMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight")
	TObjectPtr<class UMaterialInterface> outlineMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight")
	float outlineScale = 1.1f;

	//UPROPERTY(ReplicatedUsing = OnRep_Count)
	//int32 Count = 0;

	//UFUNCTION()
	//void OnRep_Count();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	//UPROPERTY(VisibleAnywhere, Category = "Components")
	//TObjectPtr<UTextRenderComponent> CountText;

	void SetHighlight(bool highlighted);

	//void UpdateCount();


};
