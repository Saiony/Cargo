// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "../Quest/QuestData.h"
#include "MissionData.generated.h"

UENUM(BlueprintType)
enum class EMissionDifficulty : uint8
{
	Easy,
	Medium,
	Hard
};

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
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Location", ToolTip = "Origin used to calculate the suggested reward."))
	FGameplayTag OriginTag;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Location"))
	FGameplayTag DestinationTag;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FCargoRequirement> CargoRequirements;	
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> UnlockRequirements;
	
	UPROPERTY(EditDefaultsOnly)
	FReward Reward;

	UPROPERTY(EditDefaultsOnly)
	EMissionDifficulty Difficulty = EMissionDifficulty::Easy;

	UFUNCTION(CallInEditor, meta = (DisplayName = "CalculateSuggestion", ToolTip = "Replace Reward Money with the calculated suggestion."))
	void CalculateSuggestion();
	
	UMissionData();
	
	FGuid GetId() const { return Id; }
	
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
};
