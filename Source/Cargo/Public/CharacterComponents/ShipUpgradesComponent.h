#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/ShipUpgrades/UpgradeCategoryType.h"
#include "ShipUpgradesComponent.generated.h"

class UShipUpgradeDA;
class USceneComponent;
class ACargoCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UShipUpgradesComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USceneComponent> ChimneyPivot;

	UPROPERTY()
	TObjectPtr<AActor> InstalledChimney;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> InstalledPaddles;

	UPROPERTY()
	TMap<EUpgradeCategoryType, TObjectPtr<UShipUpgradeDA>> InstalledUpgrades;

	UPROPERTY(Transient)
	TObjectPtr<ACargoCharacter> CargoCharacter;

	void InstallChimney(UShipUpgradeDA* UpgradeDA);
	void InstallPaddle(UShipUpgradeDA* UpgradeDA);
	void InstallHull(UShipUpgradeDA* UpgradeDA);
	void ApplyUpgradeAttributes(const UShipUpgradeDA* UpgradeDA, float Sign);

public:
	UShipUpgradesComponent();
	void Init(TObjectPtr<ACargoCharacter> InCargoCharacter);
	void SetChimneyPivot(USceneComponent* Pivot);

	UFUNCTION(BlueprintCallable, Category="Upgrades")
	void InstallUpgrade(EUpgradeCategoryType CategoryType, UShipUpgradeDA* UpgradeDA);

	UShipUpgradeDA* GetInstalledUpgrade(EUpgradeCategoryType CategoryType) const;
};
