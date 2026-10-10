#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/ShipUpgrades/UpgradeCategoryType.h"
#include "ShipUpgradesComponent.generated.h"

class UShipUpgradeDA;
class USceneComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UShipUpgradesComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USceneComponent> ChimneyPivot;

	UPROPERTY()
	TObjectPtr<AActor> InstalledChimney;

	void InstallChimney(UShipUpgradeDA* UpgradeDA);
	void InstallPaddle(UShipUpgradeDA* UpgradeDA);
	void InstallHull(UShipUpgradeDA* UpgradeDA);

public:
	UShipUpgradesComponent();
	void SetChimneyPivot(USceneComponent* Pivot);

	UFUNCTION(BlueprintCallable, Category="Upgrades")
	void InstallUpgrade(EUpgradeCategoryType CategoryType, UShipUpgradeDA* UpgradeDA);
};
