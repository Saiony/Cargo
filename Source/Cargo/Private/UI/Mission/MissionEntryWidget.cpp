// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Mission/MissionEntryWidget.h"

#include "Components/Button.h"
#include "Quest/QuestData.h"

void UMissionEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
	DefaultBorderColor = Button->GetBackgroundColor();
}

void UMissionEntryWidget::Initialize(const TObjectPtr<UMissionData> InMissionData, IMissionEntryListener* InListener, bool InIsActiveQuest)
{
	IsActiveMission = InIsActiveQuest;
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
	
	InitializeForActiveMission();
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

void UMissionEntryWidget::InitializeForActiveMission() const
{
	if (IsActiveMission)
	{
		ActiveMissionBorder->SetVisibility(ESlateVisibility::Visible);
		ExclamationBorder->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ActiveMissionBorder->SetVisibility(ESlateVisibility::Hidden);
		ExclamationBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}
