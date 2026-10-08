// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DryDock/DryDockUpgradeButton.h"

#include "CargoGameMode.h"

void UDryDockUpgradeButton::Initialize(TObjectPtr<UShipUpgradeDA> ShipUpgradeDA)
{
	Super::Initialize(ShipUpgradeDA->Name, ShipUpgradeDA->Icon);
	
	const auto PlayerMoney = ACargoGameMode::Get(this)->EconomyService->GetMoney();
	
	if (PlayerMoney < ShipUpgradeDA->Price)
	{
		MoneyDisplayWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	
	MoneyDisplayWidget->SetVisibility(ESlateVisibility::Visible);
	MoneyDisplayWidget->Initialize(ShipUpgradeDA->Price);
}
