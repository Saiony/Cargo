// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Public/Grid/FROGGrid.h"
#include "CargoGameMode.generated.h"

class APlaceable;

UCLASS(abstract)
class ACargoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:	
	ACargoGameMode();
	
	static ACargoGameMode* Get(const UObject* WorldContextObject)
	{
		return Cast<ACargoGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	};
};



