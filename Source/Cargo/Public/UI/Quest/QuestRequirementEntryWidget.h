// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "QuestRequirementEntryWidget.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class CARGO_API UQuestRequirementEntryWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> RequirementNameText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DeliveredQuantityText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> RequiredQuantityText;

public:
	void Init(FGameplayTag CargoType, int32 RequiredQuantity);
	void UpdateDelivered(int32 DeliveredQuantity);
};
