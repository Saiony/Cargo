// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestRequirementEntryWidget.h"
#include "CommonTextBlock.h"

void UQuestRequirementEntryWidget::Init(FGameplayTag CargoType, int32 RequiredQuantity)
{
	RequirementNameText->SetText(FText::FromName(CargoType.GetTagName()));
	RequiredQuantityText->SetText(FText::AsNumber(RequiredQuantity));
	UpdateDelivered(0);
}

void UQuestRequirementEntryWidget::UpdateDelivered(int32 DeliveredQuantity)
{
	DeliveredQuantityText->SetText(FText::AsNumber(DeliveredQuantity));
}
