#include "Grid/TransientGridComponent.h"

UTransientGridComponent::UTransientGridComponent()
{
	OnPlaceableRemovedFromGrid.AddDynamic(this, &UTransientGridComponent::HandlePlaceableRemovedFromGrid);
}

void UTransientGridComponent::HandlePlaceableRemovedFromGrid(APlaceable* Placeable)
{
	if (GetOccupiedSlots().IsEmpty())
	{
		GetOwner()->Destroy();
	}
}
