// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionTrackerWidget.h"

#include "CargoGameMode.h"
#include "Components/VerticalBox.h"

void UMissionTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	const auto MissionsService = ACargoGameMode::Get(this)->MissionsService;
	
	MissionsService->ActiveMissionsUpdatedDelegate.AddUObject(this, &ThisClass::OnActiveMissionsChanged);
	MissionsService->MissionProgressUpdatedDelegate.AddUObject(this, &ThisClass::OnMissionProgressUpdated);
	MissionsService->MissionCompletedDelegate.AddUObject(this, &ThisClass::OnMissionCompleted);
}

void UMissionTrackerWidget::OnActiveMissionsChanged(const TMap<FGuid, TObjectPtr<UMissionStatus>>& ActiveMissions)
{
	MissionsContainer->ClearChildren();
	MissionsEntryWidgets.Empty();

	for (const auto& ActiveMission : ActiveMissions)
	{
		auto Entry = CreateWidget<UMissionTrackerEntryWidget>(GetOwningPlayer(), MissionEntryWidgetClass);

		Entry->Init(ActiveMission.Value->GetOriginalMissionData());

		MissionsContainer->AddChild(Entry);
		MissionsEntryWidgets.Add(ActiveMission.Value->GetId(), Entry);
	}
}

void UMissionTrackerWidget::OnMissionProgressUpdated(TObjectPtr<UMissionStatus> MissionStatus, FGameplayTag CargoType)
{
	UE_LOG(LogTemp, Warning, TEXT("MissionsService: Mission progress updated"));	
	
	const auto CorrespondingWidget = MissionsEntryWidgets[MissionStatus->GetId()];
	
	if (!CorrespondingWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("MissionsService: No widget found for mission %s"), *MissionStatus->GetId().ToString());
		return;
	}
	
	CorrespondingWidget->UpdateRequirement(CargoType, MissionStatus->GetDeliveredQuantities()[CargoType].DeliveredQuantity);
}

void UMissionTrackerWidget::OnMissionCompleted(TObjectPtr<UMissionStatus> MissionStatus)
{
	UE_LOG(LogTemp, Warning, TEXT("MissionsService: Mission completed"));
	
	const auto CorrespondingWidget = MissionsEntryWidgets[MissionStatus->GetId()];
	
	if (!CorrespondingWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("MissionsService: No widget found for mission %s"), *MissionStatus->GetId().ToString());
		return;
	}
	
	CorrespondingWidget->RemoveFromParent();
}

void UMissionTrackerWidget::OnQuestEntryAnimationFinished(UMissionTrackerEntryWidget* Widget)
{	
	MissionsContainer->RemoveChild(Widget);
}
