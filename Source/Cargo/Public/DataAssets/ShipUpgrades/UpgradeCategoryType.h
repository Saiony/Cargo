#pragma once
#include "UpgradeCategoryType.generated.h"


UENUM()
enum class EUpgradeCategoryType
{
	Unknown = 0,
	Chimney,
	Paddle,
	Hull,
};

UENUM(BlueprintType)
enum class EShipUpgradeAttributeType : uint8
{
	Weight,
	Speed,
	Stability,
	MaxWeight,
	Maneuverability,
};

USTRUCT(BlueprintType)
struct FShipUpgradeAttribute
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EShipUpgradeAttributeType Type = EShipUpgradeAttributeType::Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Value = 0.f;
};
