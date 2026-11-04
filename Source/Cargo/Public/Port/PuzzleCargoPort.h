// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CargoPort.h"
#include "PuzzleCargoPort.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UPuzzleCargoPort : public UGridComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPuzzleCargoPort();

protected:		
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TObjectPtr<UGridComponentDA> PuzzleGridDA;
	
	UPROPERTY()
	TMap<FIntVector, bool> PuzzleSlots;
	TMap<TWeakObjectPtr<APlaceable>, TArray<FIntVector>> PlaceablePuzzlePositions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UInstancedStaticMeshComponent> PuzzleIMC;
	
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	
	void InitializePuzzleSlots();
	void UpdatePuzzleSlotsForPlaceable(APlaceable* Placeable, bool bOccupied);
	
	virtual void OnPlaceableAdded(APlaceable* Placeable) override;	

	UFUNCTION()
	void HandlePlaceableRemovedFromGrid(APlaceable* Placeable);
	
	void CheckIfPuzzleFinished() const;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
