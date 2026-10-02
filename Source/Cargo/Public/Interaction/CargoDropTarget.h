#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CargoDropTarget.generated.h"

class APlaceable;
class APlaceablePreview;

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UCargoDropTarget : public UInterface
{
	GENERATED_BODY()
};

/** A destination for a dragged placeable, implemented by actors or components. */
class CARGO_API ICargoDropTarget
{
	GENERATED_BODY()

public:
	virtual void BeginDropHover(APlaceable* Placeable) {}
	virtual void EndDropHover() {}
	virtual void UpdateDropHover(APlaceable* Placeable, const FVector& ImpactPoint, APlaceablePreview* Preview) = 0;

	/** Uses the cached preview placement; false keeps the object being dragged. */
	virtual bool TryAcceptDrop(APlaceable* Placeable, APlaceablePreview* Preview) = 0;
};
