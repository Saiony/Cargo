// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestEntryWidget.h"
#include "Components/VerticalBox.h"
#include "Mission/DeliveryMissionData.h"
#include "UI/Quest/CargoRequirementEntryWidget.h"

void UQuestEntryWidget::Init(FGameplayTag QuestTag, UQuestData* QuestData)
{
	if (!QuestData) 
		return;

	MyQuestTag = QuestTag;

	QuestTitleText->SetText(QuestData->Title);


	RequirementsContainer->ClearChildren();
	RequirementWidgets.Empty();

	const auto* Delivery = Cast<UDeliveryMissionData>(QuestData->MissionData);
	DestinationText->SetText(Delivery ? FText::FromName(Delivery->DestinationTag.GetTagName()) : FText::GetEmpty());
	if (!Delivery)
		return;

	for (const FCargoRequirement& Requirement : Delivery->CargoRequirements)
	{
		UCargoRequirementEntryWidget* ReqWidget = CreateWidget<UCargoRequirementEntryWidget>(this, RequirementWidgetClass);

		ReqWidget->Init(Requirement.CargoType, 0, Requirement.Quantity);
		RequirementsContainer->AddChild(ReqWidget);
		RequirementWidgets.Add(Requirement.CargoType, ReqWidget);
	}
}

void UQuestEntryWidget::UpdateRequirement(FGameplayTag CargoType, int32 DeliveredAmount)
{
	RequirementWidgets[CargoType]->UpdateDelivered(DeliveredAmount);
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
