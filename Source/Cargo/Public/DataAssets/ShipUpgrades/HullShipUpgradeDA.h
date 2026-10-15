#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeDA.h"
#include "HullShipUpgradeDA.generated.h"

class UGridComponentDA;
class UStaticMesh;

UCLASS()
class CARGO_API UHullShipUpgradeDA : public UShipUpgradeDA
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Hull")
	TObjectPtr<UStaticMesh> HullStaticMesh;

	UPROPERTY(EditDefaultsOnly, Category="Hull")
	FVector ShipFloorScale = FVector::OneVector;

	UPROPERTY(EditDefaultsOnly, Category="Hull")
	TObjectPtr<UGridComponentDA> GridComponentDA;
};
