// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/UIService.h"

#include "PrimaryGameLayout.h"
#include "DeveloperSettings/CargoSettings.h"
#include "TagDeclaration/UITypes.h"


class UPrimaryGameLayout;

UUIService::UUIService()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UUIService::Boot(FOnServiceBooted OnBootFinished)
{
	Super::Boot(OnBootFinished);	
	
	OnBootFinished.Execute();
}

UFrogsmithActivatableWidget* UUIService::ShowWidget(const TSubclassOf<UFrogsmithActivatableWidget> WidgetClass) const
{
	if (WidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UI Service: Widget class can't be null"));
		return nullptr;
	}

	UPrimaryGameLayout* Layout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	
	if (!Layout)
	{
		UE_LOG(LogTemp, Error, TEXT("UI Service: PrimaryGameLayout is unavailable"));
		return nullptr;
	}
	
	const auto Widget = Layout->PushWidgetToLayerStack<UFrogsmithActivatableWidget>(TAG_UI_Layer_Menu, WidgetClass);
	if (!Widget)
	{
		UE_LOG(LogTemp, Error, TEXT("UI Service: Failed to push widget to layer stack"));
		return nullptr;
	}
	
	return Widget;
}

UCargoInputTextWidget* UUIService::ShowInputTextWidget() const
{
	const auto InputTextWidgetClass = GetDefault<UCargoSettings>()->InputTextWidgetClass.LoadSynchronous();
	const auto InputTextWidget = ShowWidget(InputTextWidgetClass);
	
	return Cast<UCargoInputTextWidget>(InputTextWidget);
}
