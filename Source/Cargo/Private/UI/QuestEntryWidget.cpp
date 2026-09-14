// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestEntryWidget.h"
#include "Components/VerticalBox.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Mission/DeliveryMissionData.h"
#include "Mission/TravelMissionData.h"
#include "UI/Quest/CargoRequirementEntryWidget.h"

void UQuestEntryWidget::Init(FGameplayTag QuestTag, UQuestData* QuestData)
{
	if (!QuestData) 
		return;

	MyQuestTag = QuestTag;

	QuestTitleText->SetText(QuestData->Title);

	RequirementsContainer->ClearChildren();
	RequirementWidgets.Empty();
	
	if (const auto Delivery = Cast<UDeliveryMissionData>(QuestData->MissionData))
	{
		const auto DestinationName = GetDefault<UCargoSettings>()->IslandsMap.Find(Delivery->DestinationTag)->LoadSynchronous();
		DestinationText->SetText(DestinationName->DisplayName);

		for (const auto Requirement : Delivery->CargoRequirements)
		{
			UCargoRequirementEntryWidget* ReqWidget = CreateWidget<UCargoRequirementEntryWidget>(this, RequirementWidgetClass);

			ReqWidget->Init(Requirement.CargoType, 0, Requirement.Quantity);
			RequirementsContainer->AddChild(ReqWidget);
			RequirementWidgets.Add(Requirement.CargoType, ReqWidget);
		}
		
		return;
	}
	
	if (const auto TravelMission = Cast<UTravelMissionData>(QuestData->MissionData))
	{
		const auto DestinationName = GetDefault<UCargoSettings>()->IslandsMap.Find(TravelMission->DestinationTag)->LoadSynchronous();
		DestinationText->SetText(DestinationName->DisplayName);
		
		return;
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
