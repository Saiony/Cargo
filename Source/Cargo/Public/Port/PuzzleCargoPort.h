// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid/GridComponent.h"
#include "PuzzleCargoPort.generated.h"


class IPuzzleCargoPortListener;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UPuzzleCargoPort : public UGridComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPuzzleCargoPort();

protected:		
	TSet<FIntVector> PuzzleSlots;
	
	IPuzzleCargoPortListener* Listener;
	
	virtual void BeginPlay() override;
	virtual void OnPlaceableAdded(APlaceable* Placeable) override;	

	UFUNCTION()
	void HandlePlaceableRemovedFromGrid(APlaceable* Placeable);
	
	void CheckIfPuzzleFinished();

public:		
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
	
	void Init(IPuzzleCargoPortListener* InListener);
};
