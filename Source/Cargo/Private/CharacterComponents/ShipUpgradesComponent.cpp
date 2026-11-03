#include "CharacterComponents/ShipUpgradesComponent.h"

#include "DataAssets/ShipUpgrades/ShipUpgradeDA.h"
#include "DataAssets/ShipUpgrades/HullShipUpgradeDA.h"
#include "CargoCharacter.h"
#include "CharacterComponents/ShipPaddle.h"
#include "GameFramework/Pawn.h"

UShipUpgradesComponent::UShipUpgradesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UShipUpgradesComponent::Init(TObjectPtr<ACargoCharacter> InCargoCharacter)
{
	CargoCharacter = InCargoCharacter;
}

void UShipUpgradesComponent::SetChimneyPivot(USceneComponent* Pivot)
{
	ChimneyPivot = Pivot;
}

void UShipUpgradesComponent::InstallUpgrade(EUpgradeCategoryType CategoryType, UShipUpgradeDA* UpgradeDA)
{
	const auto PreviousUpgrade = GetInstalledUpgrade(CategoryType);
	
	if (PreviousUpgrade == UpgradeDA)
		return;

	switch (CategoryType)
	{
		case EUpgradeCategoryType::Chimney:
			if (!UpgradeDA->UpgradeActorClass || !IsValid(ChimneyPivot) || !GetWorld())
			{
				return;
			}
			InstallChimney(UpgradeDA);
			break;
		case EUpgradeCategoryType::Paddle:
			InstallPaddle(UpgradeDA);
			break;
		case EUpgradeCategoryType::Hull:
			InstallHull(UpgradeDA);
			break;
		default:
			return;
	}

	if (PreviousUpgrade)
	{
		CargoCharacter->RemoveWeight(PreviousUpgrade->Weight);
		ApplyUpgradeAttributes(PreviousUpgrade, -1.f);
	}

	InstalledUpgrades.Add(CategoryType, UpgradeDA);
	CargoCharacter->AddWeight(UpgradeDA->Weight);
	ApplyUpgradeAttributes(UpgradeDA, 1.f);
}

void UShipUpgradesComponent::ApplyUpgradeAttributes(const UShipUpgradeDA* UpgradeDA, const float Sign)
{
	for (const FShipUpgradeAttribute& Attribute : UpgradeDA->UpgradeData)
	{
		const float Value = Attribute.Value * Sign;
		switch (Attribute.Type)
		{
		case EShipUpgradeAttributeType::Weight:
			CargoCharacter->AddWeight(Value);
			break;
		case EShipUpgradeAttributeType::Speed:
			CargoCharacter->SetShipSpeed(CargoCharacter->GetShipBaseSpeed() + Value);
			break;
		case EShipUpgradeAttributeType::Stability:
			CargoCharacter->SetShipStability(CargoCharacter->GetShipStability() + Value);
			break;
		case EShipUpgradeAttributeType::MaxWeight:
			CargoCharacter->SetMaxWeight(CargoCharacter->GetMaxWeight() + Value);
			break;
		case EShipUpgradeAttributeType::Maneuverability:
			CargoCharacter->SetShipManeuverability(CargoCharacter->GetShipManeuverability() + Value);
			break;
		case EShipUpgradeAttributeType::FuelEfficiency:
			CargoCharacter->SetChimneyFuelEfficiency(CargoCharacter->GetChimneyFuelEfficiency() + Value);
			break;
		default:
			break;
		}
	}
}

UShipUpgradeDA* UShipUpgradesComponent::GetInstalledUpgrade(EUpgradeCategoryType CategoryType) const
{
	return InstalledUpgrades.FindRef(CategoryType);
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
	const auto Chimney = GetWorld()->SpawnActorDeferred<AActor>(UpgradeDA->UpgradeActorClass, SpawnTransform, GetOwner(),
			  Cast<APawn>(GetOwner()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Chimney->FinishSpawning(SpawnTransform);
	Chimney->AttachToComponent(ChimneyPivot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	InstalledChimney = Chimney;
}

void UShipUpgradesComponent::InstallPaddle(UShipUpgradeDA* UpgradeDA)
{
	//Remove previous paddles
	for (const auto Paddle : InstalledPaddles)
	{
		Paddle->Destroy();
	}
	
	InstalledPaddles.Empty();
	
	//Left
	const auto PaddlePivot_L = CargoCharacter->GetPaddlePivot_L();
	const auto SpawnTransform = PaddlePivot_L->GetComponentTransform();
	
	const auto Paddle_L = GetWorld()->SpawnActorDeferred<AActor>(UpgradeDA->UpgradeActorClass, SpawnTransform, GetOwner(),
			  Cast<APawn>(GetOwner()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Paddle_L->FinishSpawning(SpawnTransform);
	Paddle_L->AttachToComponent(PaddlePivot_L, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	//Right
	const auto PaddlePivot_R = CargoCharacter->GetPaddlePivot_R();
	const auto Paddle_R = GetWorld()->SpawnActorDeferred<AActor>(UpgradeDA->UpgradeActorClass, SpawnTransform, GetOwner(),
			  Cast<APawn>(GetOwner()),ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Paddle_R->FinishSpawning(SpawnTransform);
	Paddle_R->AttachToComponent(PaddlePivot_R, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	//add to array
	InstalledPaddles.Add(Paddle_L);
	InstalledPaddles.Add(Paddle_R);
}

void UShipUpgradesComponent::InstallHull(UShipUpgradeDA* UpgradeDA)
{
	const auto HullUpgradeDA = Cast<UHullShipUpgradeDA>(UpgradeDA);
	CargoCharacter->SetHull(HullUpgradeDA);
}
