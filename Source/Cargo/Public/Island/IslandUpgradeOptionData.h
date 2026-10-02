#pragma once

#include "CoreMinimal.h"
#include "Island/IslandOptionData.h"
#include "IslandUpgradeOptionData.generated.h"

UCLASS()
class CARGO_API UIslandUpgradeOptionData : public UIslandOptionData
{
	GENERATED_BODY()

public:
	virtual FText GetButtonText() const override
	{
		return NSLOCTEXT("IslandOptions", "OpenUpgrades", "Abrir Upgrades");
	}
};
