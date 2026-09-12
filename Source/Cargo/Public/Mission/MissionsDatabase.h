// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeliveryMissionData.h"
#include "Engine/DataAsset.h"
#include "MissionsDatabase.generated.h"

USTRUCT(BlueprintType)
struct FMissions
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UDeliveryMissionData>> Missions;
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
	UFUNCTION(CallInEditor, Category = "Missions", meta = (DisplayName = "Apply Locations To Mission Origins", ToolTip = "Copy each database location to the OriginTag of all missions in that group."))
	void ApplyLocationsToMissionOrigins();

	FMissions* GetMissionsForLocation(FGameplayTag LocationTag);
};
