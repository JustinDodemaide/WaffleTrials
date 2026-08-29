// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WAFFLETRIALS_API UWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Data")
	TObjectPtr<class UDataTable> ItemDataTable;
};
