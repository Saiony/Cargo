// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"
#include "Grid/GridComponent.h"
#include "Grid/Placeable.h"
#include "CargoPort.generated.h"

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
    
	virtual void BeginPlay() override;
    
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void HandlePlaceableAddedToGrid(APlaceable* Placeable);

	UFUNCTION()
	void HandlePlaceableRemovedFromGrid(APlaceable* Placeable);

public:
	UPROPERTY(VisibleAnywhere, Category="Cargo|Port")
	bool IsOpen = false;
    
	UFUNCTION(BlueprintCallable, Category = "Cargo|Port")
	FVector GetNextSpawnLocation();
    
	void DebugDrawSpawnGrid(float Duration) const;
	void AttachPlaceable(APlaceable* Placeable, FVector WorldPos);

	void AddPlaceable(APlaceable* Placeable, FVector WorldPos);

	void StartQuestDelivery(FGameplayTag QuestTag);
	void Clear();

private:
	UPROPERTY()
	int32 CurrentColumn = 0;

	UPROPERTY()
	int32 CurrentRow = 0;    
	
	FGameplayTag CurrentQuestTag;
};