// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "../Quest/QuestData.h"
#include "MissionData.generated.h"


struct FCargoRequirement;
/**
 * 
 */
UCLASS()
class CARGO_API UMissionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FGuid Id;
	
public:		
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DestinationTag;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FCargoRequirement> CargoRequirements;	
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> UnlockRequirements;
	
	UMissionData();
	
	FGuid GetId() const { return Id; }
};
