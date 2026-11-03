#pragma once

#include "CoreMinimal.h"
#include "Grid/GridComponent.h"
#include "TransientGridComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UTransientGridComponent : public UGridComponent
{
	GENERATED_BODY()

public:
	UTransientGridComponent();

private:
	UFUNCTION()
	void HandlePlaceableRemovedFromGrid(APlaceable* Placeable);
};
