// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Mission/MissionRequirementEntryWidget.h"

#include "GameplayTagContainer.h"
#include "Quest/QuestData.h"


void UMissionRequirementEntryWidget::Initialize(const FCargoRequirement& CargoRequirement)
{
	QuantityText->SetText(FText::AsNumber(CargoRequirement.Quantity));
	CommodityText->SetText(FText::FromName(CargoRequirement.CargoType.GetTagName()));
}
