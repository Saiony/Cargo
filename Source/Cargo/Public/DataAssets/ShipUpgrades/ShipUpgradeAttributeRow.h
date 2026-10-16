#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ShipUpgradeAttributeRow.generated.h"

USTRUCT(BlueprintType)
struct CARGO_API FShipUpgradeAttributeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;
};
