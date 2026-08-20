// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Grid/FROGGrid.h"
#include "GridComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaceableAddedToGrid, APlaceable*, Placeable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaceableRemovedFromGrid, APlaceable*, Placeable);

class APlaceable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UGridComponent : public UBoxComponent
{
	GENERATED_BODY()
protected:  
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FIntVector GridSize = FIntVector(1, 1, 1);

	UFROGGrid<APlaceable*> PlaceableGrid = UFROGGrid<APlaceable*>(GetDefault<UCargoSettings>()->GridCellSize, FIntVector(0, 0, 0), FIntVector(0, 0, 0));
    
	virtual void BeginPlay() override;
    
	virtual void OnPlaceableAdded(APlaceable* Placeable);

	void InitializeGrid(int32 InCellSize, const FIntVector& InOrigin, const FIntVector& InGridSize);

	FVector WorldToLocalGridSpace(const FVector& WorldLocation);

public:
	UGridComponent();
    
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
	void ClearGrid();

	bool CanAddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector WorldLocation, float Rotation);

	void AddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector& WorldLocation, float Rotation);
	
	void RemovePlaceableFromGrid(TObjectPtr<APlaceable> Placeable);

	APlaceable* GetPlaceableAt(const FIntVector WorldLocation);
	
	TMap<FIntVector, APlaceable*> GetOccupiedSlots() const;
	
	FVector GetNextFreeZPositionWorld(const FVector& WorldLocation);
	
	bool IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable);
	int32 GetHighestOccupiedZ();
	TArray<FIntVector> GetPositionsFromLevel(int Z);

	TObjectPtr<APlaceable> GetPlaceableFromLevel(int Z);

	FIntVector GetMin() const { return PlaceableGrid.GetMin(); }
	FIntVector GetMax() const { return PlaceableGrid.GetMax(); }
	float GetCellSize() const { return PlaceableGrid.GetCellSize(); }

#if !UE_BUILD_SHIPPING
	void DrawDebugGrid(float Duration = 0.f) const;
	
#endif

	FOnPlaceableAddedToGrid OnPlaceableAddedToGrid;
	FOnPlaceableRemovedFromGrid OnPlaceableRemovedFromGrid;
};