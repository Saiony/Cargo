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
	
	void Initialize(FGameplayTag InCargoType, int32 InDeliveredQuantity, int32 InTotalQuantity)
	{
		this->CargoType = InCargoType;
		this->DeliveredQuantity = InDeliveredQuantity;
		this->TotalQuantity = InTotalQuantity;
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

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TMap<FGameplayTag, FCargoStatus> DeliveredQuantities = TMap<FGameplayTag, FCargoStatus>();

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FGameplayTag QuestTag;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FText Title;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FGameplayTag StartIslandTag;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FGameplayTag DestinationTag;

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TSoftObjectPtr<UDialogueData> StartDeliveryDialogue;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TSoftObjectPtr<UDialogueData> EndDeliveryDialogue;	
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TSoftObjectPtr<UDialogueData> InProgressDialogue;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	FDialogueWithCondition AlternativeEndDeliveryDialogue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	TSoftObjectPtr<UQuestData> NextQuest; //TODO: turn this into FGameplayTag?

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UQuestData> OriginalQuestData;
};
