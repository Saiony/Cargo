// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Dialogue/DialogueData.h"
#include "Engine/DataAsset.h"
#include "Mission/BaseMissionData.h"
#include "QuestData.generated.h"

USTRUCT(BlueprintType)
struct FReward
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FGameplayTag RewardTag;
	
	UPROPERTY(EditAnywhere)
	int32 Money = 0;
};


USTRUCT(BlueprintType)
struct FCargoRequirement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Cargo"))
	FGameplayTag CargoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 Quantity = 1;
};

/**
 * 
 */
UCLASS()
class CARGO_API UQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Quest"))
	FGameplayTag QuestTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FText Title;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Location"))
	FGameplayTag StartLocationTag;
	
	/*dialogues*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UDialogueData> StartDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UDialogueData> StartDeliveryDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UDialogueData> EndDeliveryDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UDialogueData> InProgressDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDialogueWithCondition AlternativeEndDeliveryDialogue;
	/*dialogues*/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UQuestData> NextQuest; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FReward Reward;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBaseMissionData> MissionData;
};
