// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GridActorInterface/GridActorInterface.h"
#include "GridComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaceableAddedToGrid, APlaceable*, Placeable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaceableRemovedFromGrid, APlaceable*, Placeable);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UGridComponent : public UBoxComponent, public IGridActorInterface
{
	GENERATED_BODY()
protected:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FIntPoint GridSize = FIntPoint(1, 1);
	
	virtual void BeginPlay() override;
	
	virtual void OnPlaceableAdded(APlaceable* Placeable) override;	
	

public:
	UGridComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void ClearGrid();

	FOnPlaceableAddedToGrid OnPlaceableAddedToGrid;
	FOnPlaceableRemovedFromGrid OnPlaceableRemovedFromGrid;
};
