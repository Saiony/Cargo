// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Mission/MissionRequirementEntryWidget.h"

#include "GameplayTagContainer.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Quest/QuestData.h"


void UMissionRequirementEntryWidget::Initialize(const FCargoRequirement& CargoRequirement)
{
	QuantityText->SetText(FText::AsNumber(CargoRequirement.Quantity));
	const auto* CargoReference = GetDefault<UCargoSettings>()->ContainersMap.Find(CargoRequirement.CargoType);
	const auto* CargoData = CargoReference ? CargoReference->LoadSynchronous() : nullptr;
	CommodityText->SetText(CargoData ? CargoData->DisplayName : FText::GetEmpty());
}
