// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestData.h"
#include "Quest/QuestStatus.h"
#include "MissionReward.generated.h"

class UMissionStatus;

USTRUCT(BlueprintType)
struct FMissionReward
{
	GENERATED_BODY()
	
	FGameplayTag StartIslandTag;

	FGameplayTag DestinationTag;
		
	int32 MissingCargoDiscount = 0;
	
	int32 DamagedCargoDiscount = 0;
	
	int32 RecklessNavigationDiscount = 0;
	
	FReward BaseReward;
	
	FReward FinalReward;
	
	int8 Stars = 0;
	
	TMap<FGameplayTag, FCargoStatus> DeliveredQuantities;
	
	FMissionReward() = default;
	
	FMissionReward(TObjectPtr<UMissionStatus> MissionStatus);

	FReward CalculateFinalReward(TObjectPtr<UMissionStatus> MissionStatus) const;
	
	int8 CalculateStars(TObjectPtr<UMissionStatus> MissionStatus, const FReward& Reward) const;
	
	int32 CalculateDamagedCargoDiscount(TObjectPtr<UMissionStatus> MissionStatus) const;
	
	int32 CalculateMissingCargoDiscount(TObjectPtr<UMissionStatus> MissionStatus) const;

	int32 CalculateRecklessNavigationDiscount(TObjectPtr<UMissionStatus> MissionStatus) const;
};
