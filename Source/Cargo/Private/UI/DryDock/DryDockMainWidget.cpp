// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DryDock/DryDockMainWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"

void UDryDockMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CloseButton->OnClicked.AddDynamic(this, &UDryDockMainWidget::OnCloseButtonClicked);
}

void UDryDockMainWidget::OnCloseButtonClicked()
{
	ACargoGameMode::Get(this)->DryDockService->LeaveDryDock();
	Hide();
}

void UDryDockMainWidget::Hide()
{
	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}
