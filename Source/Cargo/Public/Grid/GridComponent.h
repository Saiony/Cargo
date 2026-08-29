// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "DataAssets/GridComponentDA.h"
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
	TObjectPtr<UGridComponentDA> GridComponentDA;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UInstancedStaticMeshComponent> InstancedMeshComp;
	
	UFROGGrid<APlaceable*> PlaceableGrid = UFROGGrid<APlaceable*>(GetDefault<UCargoSettings>()->GridCellSize, FIntVector(0, 0, 0), FIntVector(0, 0, 0));
    
	virtual void BeginPlay() override;
    
	virtual void OnPlaceableAdded(APlaceable* Placeable);

	void InitializeGrid();

	FVector WorldToLocal(const FVector& WorldLocation);
	
	virtual void OnRegister() override;

public:
	UGridComponent();
    
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
	void ClearGrid();

	bool CanAddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector WorldLocation, float Rotation);
	
	bool CanAddPlaceableToGridIndex(TObjectPtr<APlaceable> Placeable, FIntVector PlaceablePivotGridIndex,
	                                float Rotation);

	void AddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector& WorldLocation, float Rotation);
	void AddPlaceableToGridIndex(TObjectPtr<APlaceable> Placeable, FIntVector PlaceablePivotGridIndex, float Rotation);

	void RemovePlaceableFromGrid(TObjectPtr<APlaceable> Placeable);

	APlaceable* GetPlaceableAt(const FIntVector WorldLocation);
	
	TMap<FIntVector, APlaceable*> GetOccupiedSlots() const;
	
	FVector GetNextFreeZPositionWorld(const FVector& WorldLocation);
	
	FIntVector GetNextFreeZPositionGrid(const FVector& WorldLocation);
	FVector GridToLocalPos(FIntVector GridPos);
	FVector GetLWorldLocationFromGridIndex(FIntVector GridPos);

	bool IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable);
	int32 GetHighestOccupiedZ();
	TArray<FIntVector> GetPositionsFromLevel(int Z);

	TObjectPtr<APlaceable> GetPlaceableFromLevel(int Z);

	FIntVector GetMin() const { return PlaceableGrid.GetMin(); }
	FIntVector GetMax() const { return PlaceableGrid.GetMax(); }
	float GetCellSize() const { return PlaceableGrid.GetCellSize(); }
	
	void ShowIndicators();
	
	void HideIndicators();

#if !UE_BUILD_SHIPPING
	void DrawDebugGrid(float Duration = 0.f) const;
	
#endif

	FOnPlaceableAddedToGrid OnPlaceableAddedToGrid;
	FOnPlaceableRemovedFromGrid OnPlaceableRemovedFromGrid;
};