// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Dialogue/DialogueData.h"
#include "Engine/DataAsset.h"
#include "QuestData.generated.h"


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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo", meta = (Categories = "Quest"))
	FGameplayTag QuestTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FText Title;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo", meta = (Categories = "Location"))
	FGameplayTag StartLocationTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo", meta = (Categories = "Location"))
	FGameplayTag DestinationTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TArray<FCargoRequirement> CargoRequirements;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TSoftObjectPtr<UDialogueData> StartDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TSoftObjectPtr<UDialogueData> StartDeliveryDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TSoftObjectPtr<UDialogueData> EndDeliveryDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TSoftObjectPtr<UDialogueData> InProgressDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FDialogueWithCondition AlternativeEndDeliveryDialogue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TSoftObjectPtr<UQuestData> NextQuest; 
};
