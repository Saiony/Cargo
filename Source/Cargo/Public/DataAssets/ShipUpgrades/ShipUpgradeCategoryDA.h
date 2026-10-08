// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UpgradeCategoryType.h"
#include "Engine/DataAsset.h"
#include "ShipUpgradeCategoryDA.generated.h"

class UShipUpgradeDA;
/**
 * 
 */
UCLASS()
class CARGO_API UShipUpgradeCategoryDA : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	EUpgradeCategoryType Type;
	
	UPROPERTY(EditDefaultsOnly)
	FText Name;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UShipUpgradeDA>> ShipUpgrades;
};
