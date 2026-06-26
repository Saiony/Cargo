// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ContainerDA.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "CargoSettings.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Cargo Settings"))
class CARGO_API UCargoSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "Data Assets")
	TMap<FGameplayTag, TSoftObjectPtr<UContainerDA>> ContainersMap;
};
