// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CargoUIManagerSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Mission/DeliveryMissionStatus.h"
#include "PrimaryGameLayout.h"
#include "TagDeclaration/UITypes.h"
#include "UI/BillOfLading/BOLWidget.h"

void UCargoUIManagerSubsystem::ShowMissionResult(UDeliveryMissionStatus* MissionStatus, FSimpleDelegate OnFinished)
{
	check(IsValid(MissionStatus));
	const auto WidgetClass = GetDefault<UCargoSettings>()->BOLWidgetClass.LoadSynchronous();
	check(WidgetClass);
	auto* Layout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	check(Layout);
	auto* Widget = Layout->PushWidgetToLayerStack<UBOLWidget>(TAG_UI_Layer_Prompt, WidgetClass);
	check(Widget);
	Widget->OnFinished.Add(OnFinished);
	Widget->Init(MissionStatus);
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
