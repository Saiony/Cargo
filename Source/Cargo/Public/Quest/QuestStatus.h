// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Dialogue/DialogueData.h"
#include "Quest/QuestData.h"
#include "QuestStatus.generated.h"

USTRUCT(BlueprintType)
struct FCargoStatus
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Cargo"))
	FGameplayTag CargoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 DeliveredQuantity = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 TotalQuantity = 0;
	
	int32 BasePrice = -1;
	
	bool IsDamaged = false;
	
	void Initialize(FGameplayTag InCargoType, int32 InDeliveredQuantity, int32 InTotalQuantity, int32 InBasePrice)
	{
		CargoType = InCargoType;
		DeliveredQuantity = InDeliveredQuantity;
		TotalQuantity = InTotalQuantity;
		BasePrice = InBasePrice;
	}
	
	bool IsComplete()
	{
		return DeliveredQuantity >= TotalQuantity;
	}
};

/**
 * 
 */
UCLASS(BlueprintType)
class CARGO_API UQuestStatus : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UQuestData* QuestData);

	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	TMap<FGameplayTag, FCargoStatus> DeliveredQuantities = TMap<FGameplayTag, FCargoStatus>();

	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	FGameplayTag QuestTag;

	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	FText Title;
	
	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	FGameplayTag StartIslandTag;

	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	FGameplayTag DestinationTag;

	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	TSoftObjectPtr<UDialogueData> StartDeliveryDialogue;
	
	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	TSoftObjectPtr<UDialogueData> EndDeliveryDialogue;	
	
	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	TSoftObjectPtr<UDialogueData> InProgressDialogue;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	FDialogueWithCondition AlternativeEndDeliveryDialogue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	TSoftObjectPtr<UQuestData> NextQuest; //TODO: turn this into FGameplayTag?

	UPROPERTY(BlueprintReadOnly, Category = "Cargo")
	TObjectPtr<UQuestData> OriginalQuestData;
	
	UPROPERTY(BlueprintReadOnly, Category="Cargo")
	FReward Reward;
};
