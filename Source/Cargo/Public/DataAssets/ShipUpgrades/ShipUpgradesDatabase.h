// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShipUpgradesDatabase.generated.h"

class UShipUpgradeCategoryDA;
/**
 * 
 */
UCLASS()
class CARGO_API UShipUpgradesDatabase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UShipUpgradeCategoryDA>> ShipUpgradeCategories;
};
