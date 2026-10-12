#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeDA.h"
#include "HullShipUpgradeDA.generated.h"

class UGridComponentDA;

UCLASS()
class CARGO_API UHullShipUpgradeDA : public UShipUpgradeDA
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Hull")
	TObjectPtr<UStaticMesh> HullMesh;

	UPROPERTY(EditDefaultsOnly, Category="Hull")
	TObjectPtr<UGridComponentDA> GridComponentDA;
};
