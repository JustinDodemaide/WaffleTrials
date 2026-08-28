#pragma once

#include "CoreMinimal.h"
#include "ItemType.generated.h"

UENUM(BlueprintType)
enum class EItem : uint8
{
	None			UMETA(DisplayName = "None"),
	Bagel			UMETA(DisplayName = "Bagel"),
};