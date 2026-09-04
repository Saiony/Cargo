// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CargoUIManagerSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "DeveloperSettings/CargoSettings.h"
#include "PrimaryGameLayout.h"
#include "Services/MissionsService.h"
#include "TagDeclaration/UITypes.h"
#include "UI/BillOfLading/BOLWidget.h"

void UCargoUIManagerSubsystem::Deinitialize()
{
	if (BoundMissionsService.IsValid())
	{
		BoundMissionsService->MissionCompletedDelegate.RemoveAll(this);
	}
	BoundMissionsService.Reset();

	Super::Deinitialize();
}

void UCargoUIManagerSubsystem::BindToMissionsService(UMissionsService* MissionsService)
{
	if (BoundMissionsService.Get() == MissionsService)
	{
		return;
	}

	if (BoundMissionsService.IsValid())
	{
		BoundMissionsService->MissionCompletedDelegate.RemoveAll(this);
	}

	BoundMissionsService = MissionsService;
	if (MissionsService)
	{
		MissionsService->MissionCompletedDelegate.AddUObject(this, &ThisClass::HandleMissionCompleted);
	}
}

void UCargoUIManagerSubsystem::HandleMissionCompleted(TObjectPtr<UMissionStatus> MissionStatus)
{
	if (!IsValid(MissionStatus))
	{
		return;
	}

	const TSoftClassPtr<UBOLWidget> BOLWidgetClass = GetDefault<UCargoSettings>()->BOLWidgetClass;
	if (BOLWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoUIManagerSubsystem: BOLWidgetClass is not configured"));
		return;
	}

	UPrimaryGameLayout* Layout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	if (!Layout)
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoUIManagerSubsystem: PrimaryGameLayout is unavailable"));
		return;
	}

	if (UBOLWidget* BOLWidget = Layout->PushWidgetToLayerStack<UBOLWidget>(
		TAG_UI_Layer_Prompt, BOLWidgetClass.LoadSynchronous()))
	{
		BOLWidget->Init(MissionStatus);
	}
}

void UCargoUIManagerSubsystem::ToggleMap()
{
	bIsMapVisible ? HideMap() : ShowMap();
}

void UCargoUIManagerSubsystem::ShowMap()
{
	const auto MapClass = GetDefault<UCargoSettings>()->MapWidgetClass;
	MapWidgetInstance =  CreateWidget<UUserWidget>(GetGameInstance(), MapClass);
	
    MapWidgetInstance->AddToViewport();
    bIsMapVisible = true;
}

void UCargoUIManagerSubsystem::HideMap()
{
	MapWidgetInstance->RemoveFromParent();
	bIsMapVisible = false;
}
