// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridComponent.h"

#include "ConsoleVariables.h"
#include "Grid/Placeable.h"

UGridComponent::UGridComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGridComponent::BeginPlay()
{
	Super::BeginPlay();    
	InitializeGrid(GetDefault<UCargoSettings>()->GridCellSize, FIntVector(0, 0, 0), GridSize);
}

void UGridComponent::OnPlaceableAdded(APlaceable* Placeable)
{
}

void UGridComponent::InitializeGrid(int32 InCellSize, const FIntVector& InOrigin, const FIntVector& InGridSize)
{
	PlaceableGrid = UFROGGrid<APlaceable*>(InCellSize, InOrigin, InGridSize);
}

FVector UGridComponent::WorldToLocalGridSpace(const FVector& WorldLocation)
{
	return GetComponentTransform().InverseTransformPosition(WorldLocation);
}

bool UGridComponent::CanAddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector WorldLocation, float Rotation)
{
	if(!Placeable)
		return false;
    		
	const auto LocalLocation = WorldToLocalGridSpace(WorldLocation);

	const FVector RoundedLocation = PlaceableGrid.GetRoundedLocation(LocalLocation);
	const TArray<FVector> OccupiedGridPositions = Placeable->GetAllGridPositions(RoundedLocation, Rotation, PlaceableGrid.GetCellSize());		

	for (const FVector& Pos : OccupiedGridPositions)
	{
		const auto GridIndex = PlaceableGrid.LocalToGrid(Pos);
    
		if (PlaceableGrid.GetValue(GridIndex.X, GridIndex.Y, GridIndex.Z))
			return false;
    
		if (!PlaceableGrid.IsWithinBounds(GridIndex))
			return false;
	}

	return true;
}

void UGridComponent::AddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector& WorldLocation, float Rotation)
{
	if(!Placeable)
		return;
		
	if (!CanAddPlaceableToGrid(Placeable, WorldLocation, Rotation))
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to place Placeable on Grid"));
		return;
	}
		
	const auto LocalLocation = WorldToLocalGridSpace(WorldLocation);
	const TArray<FVector> OccupiedLocations = Placeable->GetAllGridPositions(LocalLocation, Rotation, PlaceableGrid.GetCellSize());

	for (const FVector& Pos : OccupiedLocations)
	{
		const auto GridIndex = PlaceableGrid.LocalToGrid(Pos);
		PlaceableGrid.Add(GridIndex.X, GridIndex.Y, GridIndex.Z, Placeable);

		UE_LOG(LogTemp, Log, TEXT("Placeable added to grid [%d, %d] at world pos [%f, %f]"), GridIndex.X, GridIndex.Y, Pos.X, Pos.Y);
	}

	Placeable->Place(this, LocalLocation.X, LocalLocation.Y, LocalLocation.Z);
	OnPlaceableAddedToGrid.Broadcast(Placeable);
	OnPlaceableAdded(Placeable);
}

void UGridComponent::RemovePlaceableFromGrid(TObjectPtr<APlaceable> Placeable)
{
	if(!Placeable)
		return;
		
	const auto LocalLocation = WorldToLocalGridSpace(Placeable->GetActorLocation());

	const TArray<FVector> OccupiedPos = Placeable->GetAllGridPositions(LocalLocation, Placeable->GetLocalYaw(), PlaceableGrid.GetCellSize());

	for (const FVector& Pos : OccupiedPos)
	{
		const auto GridIndex = PlaceableGrid.LocalToGrid(Pos);
		PlaceableGrid.Remove(GridIndex.X, GridIndex.Y, GridIndex.Z);
		UE_LOG(LogTemp, Log, TEXT("Placeable removed from grid [%d, %d] at local pos [%f, %f]"), GridIndex.X, GridIndex.Y, Pos.X, Pos.Y);
	}
	
	OnPlaceableRemovedFromGrid.Broadcast(Placeable);
}

TMap<FIntVector, APlaceable*> UGridComponent::GetOccupiedSlots() const
{
	return PlaceableGrid.GetOccupiedSlots();
}

FVector UGridComponent::GetNextFreeZPositionWorld(const FVector& WorldLocation)
{
	const FVector LocalLocation = WorldToLocalGridSpace(WorldLocation);
	const FVector RoundedLocation = PlaceableGrid.GetRoundedLocation(LocalLocation);

	FIntVector GridIndex = PlaceableGrid.LocalToGrid(RoundedLocation);

	while (PlaceableGrid.IsWithinBounds(GridIndex) && PlaceableGrid.GetValue(GridIndex.X, GridIndex.Y, GridIndex.Z))
	{
		GridIndex.Z++;
	}

	const float CellSize = PlaceableGrid.GetCellSize();
	const FVector NextLocalLocation = FVector(GridIndex.X * CellSize, GridIndex.Y * CellSize, GridIndex.Z * CellSize);

	return GetComponentTransform().TransformPosition(NextLocalLocation);
}

bool UGridComponent::IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable)
{
	const auto LocalLocation = WorldToLocalGridSpace(Placeable->GetActorLocation());
	const TArray<FVector> OccupiedPositions = Placeable->GetAllGridPositions(LocalLocation, Placeable->GetLocalYaw(), PlaceableGrid.GetCellSize());

	for (const FVector& Pos : OccupiedPositions)
	{
		const auto GridIndex = PlaceableGrid.LocalToGrid(Pos);
		const FIntVector AbovePos = FIntVector(GridIndex.X, GridIndex.Y, GridIndex.Z + 1);

		if (!PlaceableGrid.IsWithinBounds(AbovePos))
			continue;

		if (PlaceableGrid.GetValue(AbovePos.X, AbovePos.Y, AbovePos.Z))
			return true;
	}

	return false;
}

#if !UE_BUILD_SHIPPING
void UGridComponent::DrawDebugGrid(float Duration) const
{
	if (!GetWorld())
		return;

	const float CellSize = PlaceableGrid.GetCellSize();

	for (int32 X = PlaceableGrid.GetMin().X; X <= PlaceableGrid.GetMax().X; X++)
	{
		for (int32 Y = PlaceableGrid.GetMin().Y; Y <= PlaceableGrid.GetMax().Y; Y++)
		{
			for (int32 Z = PlaceableGrid.GetMin().Z; Z <= PlaceableGrid.GetMax().Z; Z++)
			{
				const bool bOccupied = PlaceableGrid.GetValue(X, Y, Z) != nullptr;

				const FVector LocalCellCenter = FVector(X * CellSize, Y * CellSize, Z * CellSize);
				const FVector WorldCellCenter = GetComponentTransform().TransformPosition(LocalCellCenter);

				const FColor DebugColor = bOccupied ? FColor::Red : FColor::Green;

				DrawDebugBox(GetWorld(), WorldCellCenter, FVector(CellSize * 0.4f, CellSize * 0.4f, CellSize * 0.4f),
					GetComponentQuat(), DebugColor, false, Duration, 0);
			}
		}
	}
}
#endif

void UGridComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
#if !UE_BUILD_SHIPPING
	if (CVarCargoShowDebugs.GetValueOnGameThread())
		DrawDebugGrid(0.f);
#endif
}

void UGridComponent::ClearGrid()
{
	for (auto Placeable : GetOccupiedSlots())
	{
		Placeable.Value->Destroy();
	}
    
	PlaceableGrid.Clear();
}