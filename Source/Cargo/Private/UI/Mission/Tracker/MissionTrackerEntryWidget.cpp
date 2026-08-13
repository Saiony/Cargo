// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Mission/Tracker/MissionTrackerEntryWidget.h"

#include "CommonTextBlock.h"
#include "Components/VerticalBox.h"
#include "Quest/QuestData.h"
#include "UI/Quest/CargoRequirementEntryWidget.h"


void UMissionTrackerEntryWidget::Init(TObjectPtr<UMissionData> MissionData)
{
	DestinationText->SetText(FText::FromName(MissionData->DestinationTag.GetTagName()));
	
	RequirementsContainer->ClearChildren();
	RequirementWidgets.Empty();

	for (const auto& Requirement : MissionData->CargoRequirements)
	{
		auto ReqWidget = CreateWidget<UCargoRequirementEntryWidget>(this, RequirementWidgetClass);

		ReqWidget->Init(Requirement.CargoType, Requirement.Quantity);
		RequirementsContainer->AddChild(ReqWidget);
		RequirementWidgets.Add(Requirement.CargoType, ReqWidget);
	}
}

void UMissionTrackerEntryWidget::UpdateRequirement(FGameplayTag CargoType, int32 DeliveredAmount)
{
	RequirementWidgets[CargoType]->UpdateDelivered(DeliveredAmount);
}

void UMissionTrackerEntryWidget::Complete()
{
	if (CompletedAnim)
	{
		PlayAnimation(CompletedAnim);
	}
	else
	{
		OnAnimationFinished.Broadcast(this);
	}
}

void UMissionTrackerEntryWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	
	if (Animation == CompletedAnim)
	{
		OnAnimationFinished.Broadcast(this);
	}
}