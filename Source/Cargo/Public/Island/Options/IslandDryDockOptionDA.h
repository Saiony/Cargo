// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Island/IslandOptionData.h"
#include "IslandDryDockOptionDA.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UIslandDryDockOptionDA : public UIslandOptionData
{
	GENERATED_BODY()
	
	virtual FText GetButtonText() const override
	{
		return NSLOCTEXT("IslandOptions", "Dry Dock", "Estaleiro");
	}
};
