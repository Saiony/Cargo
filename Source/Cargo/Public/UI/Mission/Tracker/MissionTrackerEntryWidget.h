// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Mission/MissionData.h"
#include "MissionTrackerEntryWidget.generated.h"

struct FGameplayTag;
class UCargoRequirementEntryWidget;
class UVerticalBox;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class CARGO_API UMissionTrackerEntryWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DestinationText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> RequirementsContainer;
	
	UPROPERTY(EditAnywhere, Category = "Cargo")
	TSubclassOf<UCargoRequirementEntryWidget> RequirementWidgetClass;
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UCargoRequirementEntryWidget>> RequirementWidgets;
	
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> CompletedAnim;
	
protected:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	
public:
	void Init(TObjectPtr<UMissionData> MissionData);
	void UpdateRequirement(FGameplayTag CargoType, int32 DeliveredAmount);
	void Complete();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestEntryAnimationFinished, UMissionTrackerEntryWidget*, Widget);
	FOnQuestEntryAnimationFinished OnAnimationFinished;
};
