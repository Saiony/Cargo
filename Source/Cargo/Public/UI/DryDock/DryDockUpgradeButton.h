// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeCategoryDA.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeDA.h"
#include "UI/Generic/GenericButton.h"
#include "UI/Generic/SimpleMoneyDisplayWidget.h"
#include "DryDockUpgradeButton.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UDryDockUpgradeButton : public UGenericButton
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USimpleMoneyDisplayWidget> MoneyDisplayWidget;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> PricePlate;
		
public:
	void Initialize(TObjectPtr<UShipUpgradeDA> ShipUpgradeDA);
};
