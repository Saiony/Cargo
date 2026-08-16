// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MissionData.h"
#include "Engine/DataAsset.h"
#include "MissionsDatabase.generated.h"

USTRUCT(BlueprintType)
struct FMissions
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMissionData>> Missions;
};

/**
 * 
 */
UCLASS()
class CARGO_API UMissionsDatabase : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Location"))
	TMap<FGameplayTag, FMissions> Database;
	
public:
	FMissions* GetMissionsForLocation(FGameplayTag LocationTag);
};
