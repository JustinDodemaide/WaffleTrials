// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "Trash.generated.h"

/**
 * 
 */
UCLASS()
class WAFFLETRIALS_API ATrash : public AStation
{
	GENERATED_BODY()
	
public:
	virtual void Interact(APawn* Interactor) override;
};
