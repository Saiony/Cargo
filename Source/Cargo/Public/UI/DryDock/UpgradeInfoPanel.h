// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "UpgradeInfoSingle.h"
#include "Components/VerticalBox.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeDA.h"
#include "UpgradeInfoPanel.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UUpgradeInfoPanel : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> Container;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TSubclassOf<UUpgradeInfoSingle> UpgradeInfoSingleClass;
	
public:
	void Initialize(TObjectPtr<UShipUpgradeDA> UpgradeDA);
};
