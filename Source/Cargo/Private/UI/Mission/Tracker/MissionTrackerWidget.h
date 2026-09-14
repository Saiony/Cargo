// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Mission/DeliveryMissionStatus.h"
#include "Services/QuestService.h"
#include "UI/Mission/MissionEntryWidget.h"
#include "UI/Mission/Tracker/MissionTrackerEntryWidget.h"
#include "MissionTrackerWidget.generated.h"

struct FGameplayTag;
class UQuestEntryWidget;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class CARGO_API UMissionTrackerWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MissionsContainer;

	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TSubclassOf<UMissionTrackerEntryWidget> MissionEntryWidgetClass;

	UPROPERTY()
	TMap<FGuid, TObjectPtr<UMissionTrackerEntryWidget>> MissionsEntryWidgets;

	virtual void NativeConstruct() override;
	
	void OnActiveMissionsChanged(const TMap<FGuid, TObjectPtr<UDeliveryMissionStatus>>& ActiveMissions);	
	void OnMissionProgressUpdated(TObjectPtr<UDeliveryMissionStatus> MissionStatus, FGameplayTag CargoType);
	void OnMissionCompleted(TObjectPtr<UDeliveryMissionStatus> MissionStatus);

	UFUNCTION()
	void OnQuestEntryAnimationFinished(UMissionTrackerEntryWidget* Widget);
	
};
