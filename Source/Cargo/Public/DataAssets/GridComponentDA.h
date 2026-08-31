// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Grid/FROGGridShapeDefinition.h"
#include "GridComponentDA.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UGridComponentDA : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> CellMesh;
	
	UPROPERTY(EditAnywhere)
	FIntVector GridSize;
		
	UPROPERTY(EditAnywhere)
	FFROGGridShapeDefinition InvalidSlots;	
};
