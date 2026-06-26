// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"
#include "Grid/Placeable.h"
#include "CargoPort.generated.h"

struct FGameplayTag;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARGO_API UCargoPortComponent : public USceneComponent, public IGridActorInterface
{
	GENERATED_BODY()
	
public:	
	UCargoPortComponent();

protected:
	//Positions relative to the ship
	UFROGGrid<APlaceable*> PlaceableGrid = UFROGGrid<APlaceable*>(GetGridCellSize(), FIntPoint(0, 0));
	
	float GetGridCellSize() const { return 100.0f; }

	
	virtual void BeginPlay() override;

	/** Área de spawn (extensão da grade) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo|Port")
	FVector SpawnAreaExtent = FVector(500.f, 500.f, 100.f);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void OnPlaceableGrabbed_Implementation(APlaceable* Placeable) override;

public:
	UPROPERTY(VisibleAnywhere, Category="Cargo|Port")
	bool IsOpen = false;
	
	UFUNCTION(BlueprintCallable, Category = "Cargo|Port")
	FVector GetNextSpawnLocation();
	
	void DebugDrawSpawnGrid(float Duration) const;
	
	void AddPlaceableToGrid(APlaceable* Placeable, FVector WorldPos);

	void StartQuestDelivery(FGameplayTag QuestTag);
	void Clear();

private:
	UPROPERTY()
	int32 CurrentColumn = 0;

	UPROPERTY()
	int32 CurrentRow = 0;
	
	UPROPERTY(EditAnywhere, Category = "Cargo|Port")
	float Spacing = 150.0f;
	
	UPROPERTY()
	TMap<FGameplayTag, int> CargoMap;  
	
	FGameplayTag CurrentQuestTag;
};
