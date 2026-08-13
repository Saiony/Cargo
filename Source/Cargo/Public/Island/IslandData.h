// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Mission/MissionData.h"
#include "IslandData.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UIslandData : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Location")
	FGameplayTag LocationTag;
};
