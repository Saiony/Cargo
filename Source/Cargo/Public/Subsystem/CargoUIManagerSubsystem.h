// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "CargoUIManagerSubsystem.generated.h"

class UUserWidget;
class UMissionsService;
class UMissionStatus;

/**
 * 
 */
UCLASS(Blueprintable)
class CARGO_API UCargoUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	UUserWidget* MapWidgetInstance;

	TWeakObjectPtr<UMissionsService> BoundMissionsService;

	void HandleMissionCompleted(TObjectPtr<UMissionStatus> MissionStatus);
	
public:
	virtual void Deinitialize() override;

	/** Connects mission-domain events to their UI presentation. */
	void BindToMissionsService(UMissionsService* MissionsService);

	void ToggleMap();
	
	void ShowMap();
	
	void HideMap();
	
	bool bIsMapVisible = false;
};
