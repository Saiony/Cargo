#include "CharacterComponents/ShipUpgradesComponent.h"

#include "DataAssets/ShipUpgrades/ShipUpgradeDA.h"
#include "GameFramework/Pawn.h"

UShipUpgradesComponent::UShipUpgradesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UShipUpgradesComponent::SetChimneyPivot(USceneComponent* Pivot)
{
	ChimneyPivot = Pivot;
}

void UShipUpgradesComponent::InstallUpgrade(EUpgradeCategoryType CategoryType, UShipUpgradeDA* UpgradeDA)
{
	if (!IsValid(UpgradeDA))
	{
		return;
	}

	switch (CategoryType)
	{
	case EUpgradeCategoryType::Chimney:
		InstallChimney(UpgradeDA);
		break;
	case EUpgradeCategoryType::Paddle:
		InstallPaddle(UpgradeDA);
		break;
	case EUpgradeCategoryType::Hull:
		InstallHull(UpgradeDA);
		break;
	default:
		break;
	}
}

void UShipUpgradesComponent::InstallChimney(UShipUpgradeDA* UpgradeDA)
{
	if (!UpgradeDA->UpgradeActorClass || !IsValid(ChimneyPivot) || !GetWorld())
	{
		return;
	}

	if (IsValid(InstalledChimney))
	{
		InstalledChimney->Destroy();
		InstalledChimney = nullptr;
	}

	const FTransform SpawnTransform = ChimneyPivot->GetComponentTransform();
	AActor* Chimney = GetWorld()->SpawnActorDeferred<AActor>(
		UpgradeDA->UpgradeActorClass,
		SpawnTransform,
		GetOwner(),
		Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Chimney)
	{
		return;
	}

	Chimney->FinishSpawning(SpawnTransform);
	Chimney->AttachToComponent(ChimneyPivot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	InstalledChimney = Chimney;
}

void UShipUpgradesComponent::InstallPaddle(UShipUpgradeDA* UpgradeDA)
{
}

void UShipUpgradesComponent::InstallHull(UShipUpgradeDA* UpgradeDA)
{
	//for now, we only adjust hull scale
	
}
