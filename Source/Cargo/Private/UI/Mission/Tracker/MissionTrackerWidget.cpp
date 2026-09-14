// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionTrackerWidget.h"

#include "CargoGameMode.h"
#include "Components/VerticalBox.h"

void UMissionTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	const auto QuestService = ACargoGameMode::Get(this)->QuestService;
	
	QuestService->ActiveMissionsUpdatedDelegate.AddUObject(this, &ThisClass::OnActiveMissionsChanged);
	QuestService->MissionProgressUpdatedDelegate.AddUObject(this, &ThisClass::OnMissionProgressUpdated);
	QuestService->MissionCompletedDelegate.AddUObject(this, &ThisClass::OnMissionCompleted);
}

void UMissionTrackerWidget::OnActiveMissionsChanged(const TMap<FGuid, TObjectPtr<UDeliveryMissionStatus>>& ActiveMissions)
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

void UMissionTrackerWidget::OnMissionProgressUpdated(TObjectPtr<UDeliveryMissionStatus> MissionStatus, FGameplayTag CargoType)
{
	UE_LOG(LogTemp, Warning, TEXT("QuestService: Mission progress updated"));	
	
	const auto CorrespondingWidget = MissionsEntryWidgets[MissionStatus->GetId()];
	
	if (!CorrespondingWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestService: No widget found for mission %s"), *MissionStatus->GetId().ToString());
		return;
	}
	
	CorrespondingWidget->UpdateRequirement(CargoType, MissionStatus->GetDeliveredQuantities()[CargoType].DeliveredQuantity);
}

void UMissionTrackerWidget::OnMissionCompleted(TObjectPtr<UDeliveryMissionStatus> MissionStatus)
{
	const auto* Widget = MissionsEntryWidgets.Find(MissionStatus->GetId());
	if (!Widget || !IsValid(*Widget))
		return;

	(*Widget)->RemoveFromParent();
	MissionsEntryWidgets.Remove(MissionStatus->GetId());
}

void UMissionTrackerWidget::OnQuestEntryAnimationFinished(UMissionTrackerEntryWidget* Widget)
{	
	MissionsContainer->RemoveChild(Widget);
}
