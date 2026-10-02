#include "Triggers/GameplayEventTrigger.h"

#include "CargoGameMode.h"

void AGameplayEventTrigger::OnPlayerEnter()
{
	ACargoGameMode::Get(this)->AddInGameEventTag(GameplayEvent);
	Destroy();
}
