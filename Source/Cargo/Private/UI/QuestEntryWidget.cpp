// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestEntryWidget.h"
#include "Components/VerticalBox.h"
#include "UI/Quest/QuestRequirementEntryWidget.h"

void UQuestEntryWidget::Init(FGameplayTag QuestTag, UQuestData* QuestData)
{
	if (!QuestData) 
		return;

	MyQuestTag = QuestTag;

	QuestTitleText->SetText(QuestData->Title);
	DestinationText->SetText(FText::FromName(QuestData->DestinationTag.GetTagName()));

	RequirementsContainer->ClearChildren();
	RequirementWidgets.Empty();

	for (const FCargoRequirement& Requirement : QuestData->CargoRequirements)
	{
		if (RequirementWidgetClass)
		{
			UQuestRequirementEntryWidget* ReqWidget = CreateWidget<UQuestRequirementEntryWidget>(this, RequirementWidgetClass);
			if (ReqWidget)
			{
				ReqWidget->Init(Requirement.CargoType, Requirement.Quantity);
				RequirementsContainer->AddChild(ReqWidget);
				RequirementWidgets.Add(Requirement.CargoType, ReqWidget);
			}
		}
	}
}

void UQuestEntryWidget::UpdateRequirement(FGameplayTag CargoType, int32 DeliveredAmount)
{
	if (RequirementWidgets.Contains(CargoType))
	{
		RequirementWidgets[CargoType]->UpdateDelivered(DeliveredAmount);
	}
}

void UQuestEntryWidget::Complete()
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

void UQuestEntryWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	
	if (Animation == CompletedAnim)
	{
		OnAnimationFinished.Broadcast(this);
	}
}
