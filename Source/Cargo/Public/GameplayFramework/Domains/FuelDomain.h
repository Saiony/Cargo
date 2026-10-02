#pragma once

#include "CoreMinimal.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFuelChanged, float, float);
DECLARE_MULTICAST_DELEGATE(FOnFuelDepleted)

class CARGO_API FFuelDomain
{
	float CurrentFuel;
	float MaxFuel;

public:
	FFuelDomain(float InCurrentFuel, float InMaxFuel);

	FOnFuelChanged OnFuelChanged;
	FOnFuelDepleted OnFuelDepleted;

	float GetCurrentFuel() const { return CurrentFuel; }
	float GetMaxFuel() const { return MaxFuel; }

	void AddFuel(float Amount);
	bool RemoveFuel(float Amount);
};
