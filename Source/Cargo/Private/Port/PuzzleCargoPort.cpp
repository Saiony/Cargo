// Fill out your copyright notice in the Description page of Project Settings.


#include "Port/PuzzleCargoPort.h"

#include "Algo/AllOf.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Grid/Placeable.h"
#include "Island/PuzzleIsland.h"


UPuzzleCargoPort::UPuzzleCargoPort()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	PuzzleIMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PuzzleIMC"));
	PuzzleIMC->SetupAttachment(this);
}


void UPuzzleCargoPort::BeginPlay()
{
	Super::BeginPlay();
}

void UPuzzleCargoPort::Init(IPuzzleCargoPortListener* InListener)
{
	OnPlaceableRemovedFromGrid.AddDynamic(this, &ThisClass::HandlePlaceableRemovedFromGrid);
	
	//we use invalid slots as valid here just because it's easier
	for (const auto ValidSlot : PuzzleGridDA->InvalidSlots.Cells)
	{
		PuzzleSlots.Add(ValidSlot, false);
	}
	
	InitializePuzzleSlots();
	
	Listener = InListener;
}

void UPuzzleCargoPort::OnRegister()
{
	Super::OnRegister();

	// Components created inside another component are not reliably registered by the owning actor.
	// Recreate the runtime component if a Blueprint archetype serialized this editable pointer as null.
	if (!IsValid(PuzzleIMC))
	{
		PuzzleIMC = NewObject<UInstancedStaticMeshComponent>(GetOwner(), TEXT("PuzzleIMC"));
		PuzzleIMC->SetupAttachment(this);
		PuzzleIMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PuzzleIMC->SetRelativeLocation(FVector(0, 0, -49)); // z = -49 so it doesn't overlap with grid component's IMC which has z = -50
		PuzzleIMC->SetGenerateOverlapEvents(false);
		PuzzleIMC->RegisterComponent();
	}
}

void UPuzzleCargoPort::InitializePuzzleSlots()
{
	PuzzleIMC->ClearInstances();
	PuzzleIMC->SetStaticMesh(PuzzleGridDA->CellMesh);
	
	for (const auto Slot : PuzzleSlots)
	{
		const auto LocalPos = GridToLocalPos(Slot.Key);
		PuzzleIMC->AddInstance(FTransform(LocalPos));
	}
}

void UPuzzleCargoPort::OnPlaceableAdded(APlaceable* Placeable)
{
	Super::OnPlaceableAdded(Placeable);
	UpdatePuzzleSlotsForPlaceable(Placeable, true);
	CheckIfPuzzleFinished();
}

void UPuzzleCargoPort::HandlePlaceableRemovedFromGrid(APlaceable* Placeable)
{
	UpdatePuzzleSlotsForPlaceable(Placeable, false);
	CheckIfPuzzleFinished();
}

void UPuzzleCargoPort::UpdatePuzzleSlotsForPlaceable(APlaceable* Placeable, bool bOccupied)
{
	if (bOccupied)
	{
		TArray<FIntVector> MatchingPositions;
		for (const auto& OccupiedSlot : GetOccupiedSlots())
		{
			if (OccupiedSlot.Value != Placeable || !PuzzleSlots.Contains(OccupiedSlot.Key))
				continue;

			PuzzleSlots[OccupiedSlot.Key] = true;
			MatchingPositions.Add(OccupiedSlot.Key);
		}

		PlaceablePuzzlePositions.Add(TWeakObjectPtr<APlaceable>(Placeable), MoveTemp(MatchingPositions));
		return;
	}

	const TArray<FIntVector>* MatchingPositions = PlaceablePuzzlePositions.Find(TWeakObjectPtr<APlaceable>(Placeable));
	if (!MatchingPositions)
		return;

	for (const FIntVector& Position : *MatchingPositions)
		PuzzleSlots[Position] = false;

	PlaceablePuzzlePositions.Remove(TWeakObjectPtr<APlaceable>(Placeable));
}

void UPuzzleCargoPort::CheckIfPuzzleFinished()
{
	const auto Finished = Algo::AllOf(PuzzleSlots, [](const auto& Slot)
	{
		return Slot.Value;
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

