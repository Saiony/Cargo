// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UpgradeCategoryType.h"
#include "Engine/DataAsset.h"
#include "ShipUpgradeDA.generated.h"


UCLASS()
class CARGO_API UShipUpgradeDA : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FText Name;
	
	UPROPERTY(EditDefaultsOnly)
	int32 Price;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly)
	FShipUpgradeData UpgradeData;
	
	UPROPERTY()
	FGuid Id = FGuid::NewGuid();
	
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
};
