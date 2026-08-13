// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Mission/MissionStatus.h"
#include "Services/MissionsService.h"
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
	
	void OnActiveMissionsChanged(const TMap<FGuid, TObjectPtr<UMissionStatus>>& ActiveMissions);	
	void OnMissionProgressUpdated(TObjectPtr<UMissionStatus> MissionStatus, FGameplayTag CargoType);
	void OnMissionCompleted(TObjectPtr<UMissionStatus> MissionStatus);

	UFUNCTION()
	void OnQuestEntryAnimationFinished(UMissionTrackerEntryWidget* Widget);
	
};
