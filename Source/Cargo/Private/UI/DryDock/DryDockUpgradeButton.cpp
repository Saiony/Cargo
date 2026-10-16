// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DryDock/DryDockUpgradeButton.h"

#include "CargoGameMode.h"

UDryDockUpgradeButton::UDryDockUpgradeButton()
{
	// Upgrade buttons are selected programmatically only after installation.
	// Letting CommonButtonBase select on click makes a pending purchase look installed.
	bSelectable = false;
	bTriggerClickedAfterSelection = true;
}

void UDryDockUpgradeButton::Initialize(TObjectPtr<UShipUpgradeDA> ShipUpgradeDA)
{
	Super::Initialize(ShipUpgradeDA->Name, ShipUpgradeDA->Icon);
	
	if (!ACargoGameMode::Get(this)->DryDockService->HasUpgradeBeenPurchased(ShipUpgradeDA->Id))
	{
		PricePlate->SetVisibility(ESlateVisibility::Visible);
		MoneyDisplayWidget->Initialize(ShipUpgradeDA->Price);
		
		return;
	}
	
	PricePlate->SetVisibility(ESlateVisibility::Hidden);	
}
