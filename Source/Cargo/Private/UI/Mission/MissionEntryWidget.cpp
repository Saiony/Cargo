// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Mission/MissionEntryWidget.h"

#include "Components/Button.h"
#include "Quest/QuestData.h"

void UMissionEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
}

void UMissionEntryWidget::Initialize(const TObjectPtr<UMissionData> InMissionData, IMissionEntryListener* InListener, bool InIsActiveQuest)
{
	IsActiveQuest = InIsActiveQuest;
	Listener = InListener;
	MissionData = InMissionData;
	
	DestinationText->SetText(FText::FromName(InMissionData->DestinationTag.GetTagName()));
	Fade();

	RequirementsContainer->ClearChildren();
	MissionRequirementEntries.Empty();

	if (!MissionRequirementEntryClass)
	{
		return;
	}

	for (const auto& MissionRequirement : InMissionData->CargoRequirements)
	{
		auto* MissionRequirementEntryWidget = CreateWidget<UMissionRequirementEntryWidget>(this, MissionRequirementEntryClass);
		if (!MissionRequirementEntryWidget)
		{
			continue;
		}
		
		MissionRequirementEntryWidget->Initialize(MissionRequirement);
		
		RequirementsContainer->AddChildToVerticalBox(MissionRequirementEntryWidget);
		MissionRequirementEntries.Add(MissionRequirementEntryWidget);
	}
}

void UMissionEntryWidget::OnButtonClicked()
{
	Listener->OnMissionEntryClicked(this);
}

void UMissionEntryWidget::Highlight()
{
	Button->SetBackgroundColor(HighlightBorderColor);
}

void UMissionEntryWidget::Fade()
{
	Button->SetBackgroundColor(DefaultBorderColor);	
}
