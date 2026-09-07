#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "IslandDistanceRow.generated.h"

USTRUCT(BlueprintType)
struct CARGO_API FIslandDistanceRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	FGameplayTag OriginIsland;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	FGameplayTag DestinationIsland;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	float Distance = 0.0f;
};
