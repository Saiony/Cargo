// Fill out your copyright notice in the Description page of Project Settings.


#include "Port/PuzzleCargoPort.h"

#include "Algo/AllOf.h"
#include "Island/PuzzleIsland.h"


UPuzzleCargoPort::UPuzzleCargoPort()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPuzzleCargoPort::BeginPlay()
{
	Super::BeginPlay();
}

void UPuzzleCargoPort::Init(IPuzzleCargoPortListener* InListener)
{
	OnPlaceableRemovedFromGrid.AddDynamic(this, &ThisClass::HandlePlaceableRemovedFromGrid);
	PuzzleSlots.Reset();
	for (int32 Y = GetMin().Y; Y <= GetMax().Y; ++Y)
		for (int32 X = GetMin().X; X <= GetMax().X; ++X)
		{
			const FIntVector Slot(X, Y, 0);
			if (!GridComponentDA->InvalidSlots.Cells.Contains(Slot))
				PuzzleSlots.Add(Slot);
		}
	
	Listener = InListener;
}

void UPuzzleCargoPort::OnPlaceableAdded(APlaceable* Placeable)
{
	Super::OnPlaceableAdded(Placeable);
	CheckIfPuzzleFinished();
}

void UPuzzleCargoPort::HandlePlaceableRemovedFromGrid(APlaceable* Placeable)
{
	CheckIfPuzzleFinished();
}

void UPuzzleCargoPort::CheckIfPuzzleFinished()
{
	const auto OccupiedSlots = GetOccupiedSlots();
	const auto Finished = !PuzzleSlots.IsEmpty() && Algo::AllOf(PuzzleSlots, [&OccupiedSlots](const FIntVector& Slot)
	{
		return OccupiedSlots.Contains(Slot);
	});
	
	if (Finished && Listener)
	{
		Listener->OnPuzzleFinished();
		Listener = nullptr;
	}
}

// Called every frame
void UPuzzleCargoPort::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

