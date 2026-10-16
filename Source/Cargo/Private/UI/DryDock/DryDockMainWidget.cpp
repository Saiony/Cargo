// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DryDock/DryDockMainWidget.h"

#include "CargoGameMode.h"
#include "CargoCharacter.h"
#include "CharacterComponents/ShipUpgradesComponent.h"
#include "PrimaryGameLayout.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBoxSlot.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeCategoryDA.h"
#include "Groups/CommonButtonGroupBase.h"
#include "UI/Generic/GenericButton.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "Core/CameraAsset.h"


class USizeBox;

void UDryDockMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CategoryButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	CategoryButtonGroup->SetSelectionRequired(true);
	
	UpgradeButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	UpgradeButtonGroup->SetSelectionRequired(true);
	
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
		
		const auto ButtonSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		ButtonSizeBox->SetWidthOverride(CategoryButtonSize);
		ButtonSizeBox->SetHeightOverride(CategoryButtonSize);
		ButtonSizeBox->AddChild(Button);
		
		const auto Slot = CategoryBox->AddChildToVerticalBox(ButtonSizeBox);
		Slot->SetPadding(FMargin(0.f, 0.f, 0.f, ButtonSpacing));
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
	SelectedCategory = Category;
	ACargoGameMode::Get(this)->DryDockService->SetSelectedCategory(Category);

	UpdateUpgradeButtons();
}

void UDryDockMainWidget::UpdateUpgradeButtons()
{
	UpgradesBox->ClearChildren();
	UpgradeButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	UpgradeButtonGroup->SetSelectionRequired(false);	
	UpgradeButtonGroup->DeselectAll();
	
	const auto ShipUpgrades = GetOwningPlayerPawn<ACargoCharacter>()->FindComponentByClass<UShipUpgradesComponent>();
	const auto InstalledUpgrade = ShipUpgrades->GetInstalledUpgrade(SelectedCategory->Type);
	
	for (const auto Upgrade : SelectedCategory->ShipUpgrades)
	{
		const auto UpgradeButton = CreateWidget<UDryDockUpgradeButton>(this, UpgradeButtonClass.Get());
		const auto Slot = UpgradesBox->AddChildToHorizontalBox(UpgradeButton);
		Slot->SetPadding(FMargin(0.f, 0.f, ButtonSpacing, 0.f));
		Slot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		Slot->SetVerticalAlignment(VAlign_Center);
		
		UpgradeButtonGroup->AddWidget(UpgradeButton);
		
		//This upgrade is the one already equipped, so select it
		if (Upgrade == InstalledUpgrade)
			UpgradeButtonGroup->SelectButtonAtIndex(UpgradeButtonGroup->GetButtonCount() - 1);
		
		UpgradeButton->Initialize(Upgrade);
		const EUpgradeCategoryType CategoryType = SelectedCategory->Type;
		UpgradeButton->OnClicked().AddWeakLambda(this, [Upgrade, CategoryType, this]()
		{
			OnUpgradeSelected(Upgrade, CategoryType);
		});
	}
	
	if (InstalledUpgrade)
	{
		UpgradeInfoPanel->Initialize(InstalledUpgrade);
		UpgradeInfoPanel->SetVisibility(ESlateVisibility::Visible);
	}
}

void UDryDockMainWidget::OnUpgradeSelected(const TObjectPtr<UShipUpgradeDA> UpgradeDA, const EUpgradeCategoryType CategoryType)
{
	UE_LOG(LogTemp, Warning, TEXT("Upgrade selected: %s"), *UpgradeDA->Name.ToString());
	UpgradeInfoPanel->Initialize(UpgradeDA);
	UpgradeInfoPanel->SetVisibility(ESlateVisibility::Visible);
	
	if (DryDockService->HasUpgradeBeenPurchased(UpgradeDA->Id))
	{
		InstallUpgrade(CategoryType, UpgradeDA);
		UpdateUpgradeButtons();
		return;
	}

	const TWeakObjectPtr WeakThis(this);
	ACargoGameMode::Get(this)->UIService->ShowSimplePurchaseWidget(UpgradeDA->Price, UpgradeDA->Name.ToString(), [WeakThis, UpgradeDA, CategoryType](const bool bPurchased)
	{
		if (!bPurchased)
			return;
		
		WeakThis->DryDockService->PurchaseUpgrade(UpgradeDA->Id);
		WeakThis->InstallUpgrade(CategoryType, UpgradeDA);
		WeakThis->UpdateUpgradeButtons();
	});
}

void UDryDockMainWidget::InstallUpgrade(const EUpgradeCategoryType CategoryType, const TObjectPtr<UShipUpgradeDA> UpgradeDA)
{
	if (ACargoCharacter* Character = GetOwningPlayerPawn<ACargoCharacter>())
	{
		Character->InstallUpgrade(CategoryType, UpgradeDA);
	}
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
