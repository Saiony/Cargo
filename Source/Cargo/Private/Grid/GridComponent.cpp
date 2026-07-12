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
	InitializeGrid(GetDefault<UCargoSettings>()->GridCellSize, FIntPoint(0, 0), GridSize);
}

void UGridComponent::OnPlaceableAdded(APlaceable* Placeable)
{
}

void UGridComponent::InitializeGrid(int32 InCellSize, const FIntPoint& InOrigin, const FIntPoint& InGridSize)
{
	PlaceableGrid = UFROGGrid<APlaceable*>(InCellSize, InOrigin, InGridSize);
}

FVector UGridComponent::WorldToLocalGridSpace(const FVector& WorldLocation)
{
	return GetComponentTransform().InverseTransformPosition(WorldLocation);
}

bool UGridComponent::CanAddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector& WorldLocation, float Rotation)
{
	if(!Placeable)
		return false;
    		
	const auto LocalLocation = WorldToLocalGridSpace(WorldLocation);

	const FVector RoundedLocation = PlaceableGrid.GetRoundedLocation(LocalLocation);
	const TArray<FVector> OccupiedGridPositions = Placeable->GetAllGridPositions(RoundedLocation, Rotation, PlaceableGrid.GetCellSize());		

	for (const FVector& Pos : OccupiedGridPositions)
	{
		const FIntPoint GridIndex = PlaceableGrid.LocalToGrid(Pos);
    
		if (PlaceableGrid.GetValue(GridIndex.X, GridIndex.Y))
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
		const FIntPoint GridIndex = PlaceableGrid.LocalToGrid(Pos);
		PlaceableGrid.Add(GridIndex.X, GridIndex.Y, Placeable);

		UE_LOG(LogTemp, Log, TEXT("Placeable added to grid [%d, %d] at world pos [%f, %f]"), GridIndex.X, GridIndex.Y, Pos.X, Pos.Y);
	}

	Placeable->Init(this, LocalLocation.X, LocalLocation.Y);
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
		const FIntPoint GridIndex = PlaceableGrid.LocalToGrid(Pos);
		PlaceableGrid.Remove(GridIndex.X, GridIndex.Y);
		UE_LOG(LogTemp, Log, TEXT("Placeable removed from grid [%d, %d] at local pos [%f, %f]"), GridIndex.X, GridIndex.Y, Pos.X, Pos.Y);
	}
}

TMap<FIntPoint, APlaceable*> UGridComponent::GetOccupiedSlots() const
{
	return PlaceableGrid.GetOccupiedSlots();
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
			const bool bOccupied = PlaceableGrid.GetValue(X, Y) != nullptr;

			const FVector LocalCellCenter = FVector(X * CellSize, Y * CellSize, 0.f);
			const FVector WorldCellCenter = GetComponentTransform().TransformPosition(LocalCellCenter);

			const FColor DebugColor = bOccupied ? FColor::Red : FColor::Green;

			DrawDebugBox(GetWorld(), WorldCellCenter, FVector(CellSize * 0.4f, CellSize * 0.4f, 1.f),
				GetComponentQuat(), DebugColor, false, Duration, 0);
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