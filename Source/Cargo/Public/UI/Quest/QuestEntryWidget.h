// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "CommonUserWidget.h"
#include "Quest/QuestData.h"
#include "QuestEntryWidget.generated.h"

class UVerticalBox;
class UCargoRequirementEntryWidget;

/**
 * 
 */
UCLASS()
class CARGO_API UQuestEntryWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> QuestTitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DestinationText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> RequirementsContainer;

	UPROPERTY(EditAnywhere, Category = "Cargo")
	TSubclassOf<UCargoRequirementEntryWidget> RequirementWidgetClass;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> CompletedAnim;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UCargoRequirementEntryWidget>> RequirementWidgets;
	
	FGameplayTag MyQuestTag;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestEntryAnimationFinished, UQuestEntryWidget*, Widget);
	FOnQuestEntryAnimationFinished OnAnimationFinished;

	void Init(FGameplayTag QuestTag, UQuestData* QuestData);
	void UpdateRequirement(FGameplayTag CargoType, int32 DeliveredAmount);
	void Complete();

	FGameplayTag GetQuestTag() const { return MyQuestTag; }

protected:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
};
