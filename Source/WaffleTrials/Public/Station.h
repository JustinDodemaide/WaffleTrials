
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Station.generated.h"

class UTextRenderComponent;

UCLASS()
class WAFFLETRIALS_API AStation : public AActor
{
	GENERATED_BODY()
	
public:	
	AStation();
	void Use();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Use)
	int32 Count = 0;

	UFUNCTION()
	void OnRep_Use();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UTextRenderComponent> CountText;

	void SetHighlight(bool highlighted);
};
