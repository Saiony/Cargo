#pragma once

#include "CoreMinimal.h"
#include "PlayerConfigDA.generated.h"

class UHullShipUpgradeDA;
class UShipUpgradeDA;

UCLASS()
class CARGO_API UPlayerConfigDA : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Player|Initial Upgrades")
	TObjectPtr<UHullShipUpgradeDA> InitialHull;

	UPROPERTY(EditDefaultsOnly, Category="Player|Initial Upgrades")
	TObjectPtr<UShipUpgradeDA> InitialChimney;
	
	UPROPERTY(EditDefaultsOnly, Category="Player|Initial Upgrades")
	TObjectPtr<UShipUpgradeDA> InitialPaddle;
};
