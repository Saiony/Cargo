#include "GameplayFramework/Domains/FuelDomain.h"

FFuelDomain::FFuelDomain(float InCurrentFuel, float InMaxFuel) : CurrentFuel(InCurrentFuel), MaxFuel(InMaxFuel)
{
}

void FFuelDomain::AddFuel(const float Amount)
{	
	CurrentFuel = FMath::Min(CurrentFuel + Amount, MaxFuel);
	OnFuelChanged.Broadcast(CurrentFuel, MaxFuel);
}

bool FFuelDomain::RemoveFuel(const float Amount)
{
	if (CurrentFuel <= 0)
		return false;
	
	const auto PreviousFuel = CurrentFuel;
	CurrentFuel -= Amount;
	
	if (static_cast<int32>(CurrentFuel) != static_cast<int32>(PreviousFuel)) //if changed the integer value
		OnFuelChanged.Broadcast(CurrentFuel, MaxFuel);
	
	if (CurrentFuel <= 0)
	{
		CurrentFuel = 0;
		OnFuelDepleted.Broadcast();
		
		return false;
	}
	
	return true;
}
