// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/ShipUpgrades/ShipUpgradeDA.h"

void UShipUpgradeDA::PostDuplicate(const bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
	
	Id = FGuid::NewGuid();
}
