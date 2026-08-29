// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"
#include "Grid/GridComponent.h"
#include "CargoPort.generated.h"

struct FCargoRequirement;
class AContainer;
struct FGameplayTag;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARGO_API UCargoPortComponent : public UGridComponent
{
	GENERATED_BODY()
    
public: 
	UCargoPortComponent();

protected:  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UGridComponent> GridComp;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo|Island")
	TSubclassOf<AContainer> ContainerClass;	
    
	virtual void BeginPlay() override;
    
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void HandlePlaceableAddedToGrid(APlaceable* Placeable);

	UFUNCTION()
	void HandlePlaceableRemovedFromGrid(APlaceable* Placeable);
	
	void SpawnSingleContainer(FGameplayTag CargoType);

public:
	UPROPERTY(VisibleAnywhere, Category="Cargo|Port")
	bool IsOpen = false;
    
	void DebugDrawSpawnGrid(float Duration) const;
	
	void AttachPlaceable(APlaceable* Placeable, FVector WorldPos);

	void AddPlaceable(APlaceable* Placeable, FVector WorldPos, float Rotation);
	void AddPlaceableIndex(APlaceable* Placeable, FIntVector Index, float Rotation);

	void StartQuestDelivery(FGameplayTag QuestTag);
	
	void StartMissionDelivery(FGuid MissionId);

	void Clear();
	
	void SpawnCargo(const TArray<FCargoRequirement>& Requirements);

private:	
	FGameplayTag CurrentQuestTag;
	
	FGuid CurrentMissionId;
};