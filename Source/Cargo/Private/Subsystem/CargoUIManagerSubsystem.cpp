// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CargoUIManagerSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "DeveloperSettings/CargoSettings.h"

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
