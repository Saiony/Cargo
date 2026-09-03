// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/CargoRequirementEntryWidget.h"
#include "CommonTextBlock.h"
#include "Mission/MissionStatus.h"

void UCargoRequirementEntryWidget::Init(FGameplayTag CargoType, int32 DeliveredQuantity, int32 RequiredQuantity)
{
	RequirementNameText->SetText(FText::FromName(CargoType.GetTagName()));
	RequiredQuantityText->SetText(FText::AsNumber(RequiredQuantity));
	UpdateDelivered(0);
}

void UCargoRequirementEntryWidget::Init(const FCargoStatus& CargoStatus)
{
	Init(CargoStatus.CargoType, CargoStatus.DeliveredQuantity, CargoStatus.TotalQuantity);
}

void UCargoRequirementEntryWidget::UpdateDelivered(int32 DeliveredQuantity)
{
	DeliveredQuantityText->SetText(FText::AsNumber(DeliveredQuantity));
}
