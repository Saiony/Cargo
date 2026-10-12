#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/ShipUpgrades/UpgradeCategoryType.h"
#include "ShipUpgradesComponent.generated.h"

class UShipUpgradeDA;
class USceneComponent;

USTRUCT(BlueprintType)
struct FInstalledShipUpgrade
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EUpgradeCategoryType Category = EUpgradeCategoryType::Unknown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UShipUpgradeDA> UpgradeDA;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UShipUpgradesComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USceneComponent> ChimneyPivot;

	UPROPERTY()
	TObjectPtr<AActor> InstalledChimney;

	UPROPERTY()
	TArray<FInstalledShipUpgrade> InstalledUpgrades;

	void InstallChimney(UShipUpgradeDA* UpgradeDA);
	void InstallPaddle(UShipUpgradeDA* UpgradeDA);
	void InstallHull(UShipUpgradeDA* UpgradeDA);

public:
	UShipUpgradesComponent();
	void SetChimneyPivot(USceneComponent* Pivot);

	UFUNCTION(BlueprintCallable, Category="Upgrades")
	void InstallUpgrade(EUpgradeCategoryType CategoryType, UShipUpgradeDA* UpgradeDA);

	UShipUpgradeDA* GetInstalledUpgrade(EUpgradeCategoryType CategoryType) const;
};
