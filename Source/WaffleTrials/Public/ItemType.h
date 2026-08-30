#pragma once

#include "CoreMinimal.h"
#include "ItemType.generated.h"

UENUM(BlueprintType)
enum class EItem : uint8
{
	None			UMETA(DisplayName = "None"),
	Bagel			UMETA(DisplayName = "Bagel"),
	Waffle			UMETA(DisplayName = "None"),
	Rubbish			UMETA(DisplayName = "Bagel"),
	Coffee			UMETA(DisplayName = "None"),
	//Bagel			UMETA(DisplayName = "Bagel"),
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	TObjectPtr<class UPaperSprite> sprite;
};