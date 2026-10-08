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

USTRUCT()
struct FShipUpgradeData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	float UpgradeWeight = 0.f;
	
	UPROPERTY(EditDefaultsOnly)
	float Speed = 0.f;	

	UPROPERTY(EditDefaultsOnly)
	float Stability = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxWeight = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Maneuverability = 0.f;
};
