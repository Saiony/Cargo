// Copyright Epic Games, Inc. All Rights Reserved.

#include "Whale/AutoDestructible.h"

AAutoDestructible::AAutoDestructible()
{
	PrimaryActorTick.bCanEverTick = false;
	InitialLifeSpan = 2.0f;
}
