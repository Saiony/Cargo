#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "StoreCatalog.generated.h"

UCLASS()
class CARGO_API UStoreCatalog : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGameplayTag ContainerId;

	UPROPERTY(EditAnywhere)
	int32 Price = 0;
};
