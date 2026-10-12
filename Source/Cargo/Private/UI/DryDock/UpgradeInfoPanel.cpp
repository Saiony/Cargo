// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DryDock/UpgradeInfoPanel.h"

#include "DataAssets/ShipUpgrades/ShipUpgradeAttributeRow.h"
#include "DeveloperSettings/CargoSettings.h"

void UUpgradeInfoPanel::Initialize(TObjectPtr<UShipUpgradeDA> UpgradeDA)
{
	Container->ClearChildren();
	const UDataTable* AttributeTable = GetDefault<UCargoSettings>()->ShipUpgradeAttributeTable.LoadSynchronous();

	const auto AddAttribute = [this, AttributeTable](EShipUpgradeAttributeType Type, float Value)
	{
		const FName RowName(*StaticEnum<EShipUpgradeAttributeType>()->GetNameStringByValue(static_cast<int64>(Type)));
		const FShipUpgradeAttributeRow* AttributeInfo = AttributeTable->FindRow<FShipUpgradeAttributeRow>(RowName, TEXT("UpgradeInfoPanel"));
		
		if (!AttributeInfo)
		{
			const FString TypeName = StaticEnum<EShipUpgradeAttributeType>()->GetNameStringByValue(static_cast<int64>(Type));
			UE_LOG(LogTemp, Error, TEXT("Failed to find attribute info for type %s"), *TypeName);
			return;
		}
		
		const auto InfoWidget = CreateWidget<UUpgradeInfoSingle>(this, UpgradeInfoSingleClass);
		InfoWidget->Initialize(AttributeInfo->Name, AttributeInfo->Icon, Value);
		Container->AddChild(InfoWidget);
	};

	AddAttribute(EShipUpgradeAttributeType::Weight, UpgradeDA->Weight);
	for (const FShipUpgradeAttribute& Attribute : UpgradeDA->UpgradeData)
	{
		AddAttribute(Attribute.Type, Attribute.Value);
	}
}
