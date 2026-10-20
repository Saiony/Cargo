// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutoDestructible.generated.h"

/** Blueprint-configurable short-lived actor for an impact visual. */
UCLASS()
class AAutoDestructible : public AActor
{
	GENERATED_BODY()

public:
	AAutoDestructible();
};
