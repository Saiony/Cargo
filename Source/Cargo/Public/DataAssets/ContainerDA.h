// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Grid/FROGGridShapeDefinition.h"
#include "ContainerDA.generated.h"

class APlaceableVisual;
/**
 * 
 */
UCLASS()
class CARGO_API UContainerDA : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Cargo"))
	FGameplayTag CargoTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name = "Debug";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> Material;
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight = 1.0f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BasePrice = 10;
	
	UPROPERTY(EditAnywhere)
	FFROGGridShapeDefinition Shape;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<APlaceableVisual> VisualBPClass;
};
