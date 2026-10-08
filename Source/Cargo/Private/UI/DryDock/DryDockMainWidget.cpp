// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DryDock/DryDockMainWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeCategoryDA.h"
#include "Groups/CommonButtonGroupBase.h"
#include "UI/Generic/GenericButton.h"


void UDryDockMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CategoryButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	CategoryButtonGroup->SetSelectionRequired(true);
	
	UpgradeButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	CategoryButtonGroup->SetSelectionRequired(true);
	
	CloseButton->OnClicked.AddDynamic(this, &UDryDockMainWidget::OnCloseButtonClicked);
	
	const auto UpgradesDatabase = GetDefault<UCargoSettings>()->ShipUpgradesDatabase.LoadSynchronous();
	CreateCategoryButtons(UpgradesDatabase);
	
	CategoryButtonGroup->SelectButtonAtIndex(0, false); //TODO: pegar selecao do upgrade ja instalado
	OnCategorySelected(UpgradesDatabase->ShipUpgradeCategories[0]);	
}

void UDryDockMainWidget::CreateCategoryButtons(TObjectPtr<UShipUpgradesDatabase> UpgradesDatabase)
{
	const auto GenericButtonClass = GetDefault<UCargoSettings>()->GenericButtonClass.LoadSynchronous();
	
	for (const auto Category : UpgradesDatabase->ShipUpgradeCategories)
	{
		const auto Button = CreateWidget<UGenericButton>(this, GenericButtonClass);
		CategoryBox->AddChild(Button);
		CategoryButtonGroup->AddWidget(Button);		
		
		Button->Initialize(FText::GetEmpty(), Category->Icon);
		Button->OnClicked().AddWeakLambda(this, [this, Category]()
		{
			OnCategorySelected(Category);
		});
	}
}

void UDryDockMainWidget::OnCategorySelected(const TObjectPtr<UShipUpgradeCategoryDA> Category)
{			
	UE_LOG(LogTemp, Warning, TEXT("Category selected: %s"), *Category->Name.ToString());
	SelectedCategory = Category;
	UpdateUpgradeButtons();
}

void UDryDockMainWidget::UpdateUpgradeButtons()
{
	UpgradesBox->ClearChildren();
	
	for (const auto& Upgrade : SelectedCategory->ShipUpgrades)
	{
		const auto UpgradeButton = CreateWidget<UDryDockUpgradeButton>(this, UpgradeButtonClass.Get());
		UpgradesBox->AddChildToHorizontalBox(UpgradeButton);
		UpgradeButton->Initialize(Upgrade);
		
		UpgradeButton->OnClicked().AddWeakLambda(this, [Upgrade, this]()
		{
			OnUpgradeSelected(Upgrade);
		});
	}
}

void UDryDockMainWidget::OnUpgradeSelected(const TObjectPtr<UShipUpgradeDA> UpgradeDA)
{
	UE_LOG(LogTemp, Warning, TEXT("Upgrade selected: %s"), *UpgradeDA->Name.ToString());
	
	//TODO: se nao tiver comprado, aparecer modal de compra
	// se ja tiver comprado, instalar upgrade no barco	
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
