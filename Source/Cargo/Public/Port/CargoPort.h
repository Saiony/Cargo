// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"
#include "Grid/GridComponent.h"
#include "CargoPort.generated.h"

struct FCargoRequirement;
class AContainer;
class APortBellInteractable;
struct FGameplayTag;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARGO_API UCargoPortComponent : public UGridComponent
{
	GENERATED_BODY()
    
public: 
	UCargoPortComponent();

protected:  
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TSubclassOf<AContainer> ContainerClass;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Port|Interaction")
	TSubclassOf<APortBellInteractable> PortBellClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Port|Interaction")
	FTransform PortBellRelativeTransform;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void HandlePlaceableAddedToGrid(APlaceable* Placeable);

	UFUNCTION()
	void HandlePlaceableRemovedFromGrid(APlaceable* Placeable);
	

	UFUNCTION()
	void OnBellClicked();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category="Cargo|Port|Interaction")
	TObjectPtr<APortBellInteractable> PortBell;

public:
	UPROPERTY(VisibleAnywhere, Category="Cargo|Port")
	bool IsOpen = false;
	
	bool IsPickup = false;
    
	void DebugDrawSpawnGrid(float Duration) const;
	
	void AttachPlaceable(APlaceable* Placeable, FVector WorldPos);

	void AddPlaceable(APlaceable* Placeable, FVector WorldPos, float Rotation);
	
	void AddPlaceableIndex(APlaceable* Placeable, FIntVector Index, float Rotation);
	
	void StartMissionDelivery(FGuid MissionId);

	void Clear();
	
	void SpawnCargo(const TArray<FCargoRequirement>& Requirements);
	void SpawnSingleContainer(FGameplayTag CargoType);

	void OpenPort();
	
	void OpenPortForPickup();

	void ClosePort();
private:		
	FGuid CurrentMissionId;
	
};
