// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Grid/FROGGrid.h"
#include "Grid/Placeable.h"
#include "UObject/Interface.h"
#include "MyClass.generated.h"


class APlaceable;

UINTERFACE(MinimalAPI, Blueprintable)
class UGridActorInterface : public UInterface { GENERATED_BODY() };

class IGridActorInterface
{
	GENERATED_BODY()
protected:	
	UFROGGrid<APlaceable*> PlaceableGrid = UFROGGrid<APlaceable*>(GetDefault<UCargoSettings>()->GridCellSize, FIntPoint(0, 0), FIntPoint(0, 0));
	
	void InitializeGrid(int32 InCellSize, const FIntPoint& InOrigin, const FIntPoint& InGridSize)
	{
		PlaceableGrid = UFROGGrid<APlaceable*>(InCellSize, InOrigin, InGridSize);
	}
	
	virtual void OnPlaceableAdded(APlaceable* Placeable){}
	
	AActor* GetOwningActor()
	{
		return Cast<AActor>(Cast<UObject>(this));
	}

	FVector WorldToLocalGridSpace(const FVector& WorldLocation)
	{
		return GetOwningActor()->GetActorTransform().InverseTransformPosition(WorldLocation);
	}
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Notify")
	void OnPlaceableGrabbed(APlaceable* Placeable);
	
	bool CanAddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector& WorldLocation, float Rotation)
	{
		if(!Placeable)
			return false;
    		
		const auto LocalLocation = WorldToLocalGridSpace(WorldLocation);

		const FVector RoundedLocation = PlaceableGrid.GetRoundedLocation(LocalLocation);
		const TArray<FVector> OccupiedGridPositions = Placeable->GetAllGridPositions(RoundedLocation, Rotation, PlaceableGrid.GetCellSize());		

		for (const FVector& Pos : OccupiedGridPositions)
		{
			const FIntPoint GridIndex = PlaceableGrid.LocalToGrid(Pos);
    
			if (PlaceableGrid.GetValue(GridIndex.X, GridIndex.Y)) //check already occupied locations
				return false;
    
			if (!PlaceableGrid.IsWithinBounds(GridIndex)) //check if out of bounds
				return false;
		}

		return true;
	}

	void AddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector& WorldLocation, float Rotation)
	{
		if(!Placeable)
			return;
		
		const auto LocalLocation = WorldToLocalGridSpace(WorldLocation);
		
		if (!CanAddPlaceableToGrid(Placeable, LocalLocation, Rotation))
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to place Placeable on Grid"));
			return;
		}
		
		const TArray<FVector> OccupiedLocations = Placeable->GetAllGridPositions(LocalLocation, Rotation, PlaceableGrid.GetCellSize());

		for (const FVector& Pos : OccupiedLocations)
		{
			const FIntPoint GridIndex = PlaceableGrid.LocalToGrid(Pos);
			PlaceableGrid.Add(GridIndex.X, GridIndex.Y, Placeable);

			UE_LOG(LogTemp, Log, TEXT("Placeable added to grid [%d, %d] at world pos [%f, %f]"), GridIndex.X, GridIndex.Y, Pos.X, Pos.Y);
		}

		Placeable->Init(GetOwningActor(), LocalLocation.X, LocalLocation.Y);
		OnPlaceableAdded(Placeable);
	}
	
	void RemovePlaceableFromGrid(TObjectPtr<APlaceable> Placeable)
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
	
#if !UE_BUILD_SHIPPING
	void DrawDebugGrid(float Duration = 0.f) const
	{
		auto OwningActor = Cast<AActor>(Cast<UObject>(this));
		if (!OwningActor || !OwningActor->GetWorld())
			return;

		UWorld* World = OwningActor->GetWorld();
		const float CellSize = PlaceableGrid.GetCellSize();

		for (int32 X = PlaceableGrid.GetMin().X; X <= PlaceableGrid.GetMax().X; X++)
		{
			for (int32 Y = PlaceableGrid.GetMin().Y; Y <= PlaceableGrid.GetMax().Y; Y++)
			{
				const bool bOccupied = PlaceableGrid.GetValue(X, Y) != nullptr;

				const FVector LocalCellCenter = FVector(X * CellSize, Y * CellSize, 0.f);
				const FVector WorldCellCenter = OwningActor->GetActorTransform().TransformPosition(LocalCellCenter) + FVector(0, 0, 150);

				const FColor DebugColor = bOccupied ? FColor::Red : FColor::Green;

				DrawDebugBox(World, WorldCellCenter, FVector(CellSize * 0.4f, CellSize * 0.4f, 5.f),
					OwningActor->GetActorQuat(), DebugColor, false, Duration, 0);
			}
		}
	}
#endif
};
