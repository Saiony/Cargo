// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DryDock/DryDockMainWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeCategoryDA.h"
#include "Groups/CommonButtonGroupBase.h"
#include "UI/Generic/GenericButton.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "Core/CameraAsset.h"


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
	
	DryDockService = ACargoGameMode::Get(this)->DryDockService;
}

void UDryDockMainWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	const auto PlayerPawn = GetOwningPlayerPawn();
	ShipCameraComponent = PlayerPawn->FindComponentByClass<UGameplayCameraComponent>();
	check(ShipCameraComponent);
	
	GameplayCameraAsset = ShipCameraComponent->CameraReference.GetCameraAsset();
	SetCameraAsset(DryDockCameraAsset);

	const auto UpgradesDatabase = GetDefault<UCargoSettings>()->ShipUpgradesDatabase.LoadSynchronous();
	check(UpgradesDatabase);
	
	CategoryButtonGroup->SelectButtonAtIndex(0, false);
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
	ACargoGameMode::Get(this)->DryDockService->SetSelectedCategory(Category);

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
	
	if (DryDockService->HasUpgradeBeenPurchased(UpgradeDA->Id))
	{
		InstallUpgrade(UpgradeDA);
		return;
	}
	
	const TWeakObjectPtr WeakThis(this);
	ACargoGameMode::Get(this)->UIService->ShowSimplePurchaseWidget(UpgradeDA->Price, UpgradeDA->Name.ToString(), [WeakThis, UpgradeDA](const bool bPurchased)
	{
		if (!bPurchased)
			return;
		
		WeakThis->DryDockService->PurchaseUpgrade(UpgradeDA->Id);
		WeakThis->UpdateUpgradeButtons();
	});
}

void UDryDockMainWidget::InstallUpgrade(TObjectPtr<UShipUpgradeDA> UpgradeDA)
{
	
}

void UDryDockMainWidget::OnCloseButtonClicked()
{
	RestoreGameplayCamera();
	ACargoGameMode::Get(this)->DryDockService->LeaveDryDock();
	Hide();
}

void UDryDockMainWidget::SetCameraAsset(UCameraAsset* CameraAsset) const
{
	if (!ShipCameraComponent || !CameraAsset || ShipCameraComponent->CameraReference.GetCameraAsset() == CameraAsset)
	{
		return;
	}

	ShipCameraComponent->DeactivateCamera(true);
	ShipCameraComponent->CameraReference.SetCameraAsset(CameraAsset);
	ShipCameraComponent->ActivateCameraForPlayerController(GetOwningPlayer(), true);
}

void UDryDockMainWidget::RestoreGameplayCamera()
{
	SetCameraAsset(GameplayCameraAsset);
	ShipCameraComponent = nullptr;
	GameplayCameraAsset = nullptr;
}

void UDryDockMainWidget::Hide()
{
	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}
