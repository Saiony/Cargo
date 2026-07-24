// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InputDisplayWidget.h"

#include "CargoPlayerController.h"

void UInputDisplayWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();	
	
	SetVisibility(ESlateVisibility::Hidden);
	
	auto PC = Cast<ACargoPlayerController>(GetOwningPlayer());
	PC->OnInteractableChanged.AddDynamic(this, &ThisClass::HandleInteractableChanged);	
}

void UInputDisplayWidget::HandleInteractableChanged(TScriptInterface<ICargoInteractable> NewInteractable)
{
	SetVisibility(ESlateVisibility::Visible);
	if (NewInteractable == nullptr)
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	else
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
}
