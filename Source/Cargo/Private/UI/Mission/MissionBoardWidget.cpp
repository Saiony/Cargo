// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Mission/MissionBoardWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"
#include "Components/Button.h"
#include "Mission/MissionsDatabase.h"


void UMissionBoardWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	AcceptMissionButton->OnClicked.AddDynamic(this, &ThisClass::OnAcceptMissionButtonClicked);
	AcceptMissionButton->SetVisibility(ESlateVisibility::Hidden);
	
	DeliverMissionButton->OnClicked.AddDynamic(this, &ThisClass::OnDeliverMissionButtonClicked);
	DeliverMissionButton->SetVisibility(ESlateVisibility::Hidden);
	
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
	CloseButton2->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
}

void UMissionBoardWidget::Initialize(FMissions* MissionData, TObjectPtr<ACargoIsland> InInstigatorIsland)
{
	InstigatorIsland = InInstigatorIsland;
	ClearBoard();
	
	ShowAvailableMissions(MissionData);
	ShowActiveMissions();
}

void UMissionBoardWidget::ShowAvailableMissions(FMissions* MissionsData)
{
	if (!MissionsData)
	{
		UE_LOG(LogTemp, Log, TEXT("MissionBoardWidget: No missions to display"));
		return;
	}	
	
	for (int i = 0; i < MissionsData->Missions.Num(); i++)	
	{
		const auto Mission = MissionsData->Missions[i];
		
		const auto MissionEntryWidget = CreateWidget<UMissionEntryWidget>(this, MissionEntryClass);
		MissionEntryWidget->Initialize(Mission, this, false);
		
		const auto GridSlot = MissionsContainer->AddChildToWrapBox(MissionEntryWidget);		
	}
}

void UMissionBoardWidget::ShowActiveMissions()
{
	const auto MissionsService = ACargoGameMode::Get(this)->MissionsService;
	auto ActiveMissions = MissionsService->GetActiveMissionsForDestination(InstigatorIsland->GetLocationTag());
	
	for (auto ActiveMission : ActiveMissions)
	{
		const auto MissionEntryWidget = CreateWidget<UMissionEntryWidget>(this, MissionEntryClass);
		MissionEntryWidget->Initialize(ActiveMission->GetOriginalMissionData(), this, true);
		
		MissionsContainer->AddChildToWrapBox(MissionEntryWidget);		
	}
}

void UMissionBoardWidget::ClearBoard()
{
	MissionsContainer->ClearChildren();
}

void UMissionBoardWidget::OnMissionEntryClicked(UMissionEntryWidget* MissionEntryWidget)
{
	if (SelectedWidget == MissionEntryWidget)
		return;
	
	if (SelectedWidget)
		SelectedWidget->Fade();
	
	SelectedWidget = MissionEntryWidget;
	SelectedWidget->Highlight();
	
	if (MissionEntryWidget->IsActiveMission)
	{
		DeliverMissionButton->SetVisibility(ESlateVisibility::Visible);
		AcceptMissionButton->SetVisibility(ESlateVisibility::Hidden);		
	}
	else
	{
		DeliverMissionButton->SetVisibility(ESlateVisibility::Hidden);		
		AcceptMissionButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMissionBoardWidget::OnAcceptMissionButtonClicked()
{
	const auto AcceptedMission = SelectedWidget->MissionData;
	ACargoGameMode::Get(this)->MissionsService->AcceptMission(AcceptedMission, InstigatorIsland->GetLocationTag());
	
	CloseWidget();
}

void UMissionBoardWidget::OnDeliverMissionButtonClicked()
{
	const auto AcceptedMission = SelectedWidget->MissionData;	
	InstigatorIsland->GetPort()->StartMissionDelivery(SelectedWidget->MissionData->GetId());
	
	CloseWidget();
}

void UMissionBoardWidget::OnCloseButtonClicked()
{
	CloseWidget();
}

void UMissionBoardWidget::CloseWidget()
{
	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}