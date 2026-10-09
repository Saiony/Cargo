// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DryDockUpgradeButton.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "DataAssets/ShipUpgrades/ShipUpgradesDatabase.h"
#include "DataAssets/ShipUpgrades/UpgradeCategoryType.h"
#include "DryDockMainWidget.generated.h"

class UCommonButtonGroupBase;
class UCameraAsset;
class UGameplayCameraComponent;
/**
 * 
 */
UCLASS()
class CARGO_API UDryDockMainWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CategoryBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> UpgradesBox;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TSubclassOf<UDryDockUpgradeButton> UpgradeButtonClass;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	TObjectPtr<UCameraAsset> DryDockCameraAsset;

	UPROPERTY()
	TObjectPtr<UGameplayCameraComponent> ShipCameraComponent;

	UPROPERTY()
	TObjectPtr<UCameraAsset> GameplayCameraAsset;
	
	UPROPERTY()
	TObjectPtr<UCommonButtonGroupBase> CategoryButtonGroup;
	
	UPROPERTY()
	TObjectPtr<UCommonButtonGroupBase> UpgradeButtonGroup;
	
	UPROPERTY()
	TObjectPtr<UShipUpgradeCategoryDA> SelectedCategory;
	
	void CreateCategoryButtons(TObjectPtr<UShipUpgradesDatabase> UpgradesDatabase);
	
	void OnCategorySelected(TObjectPtr<UShipUpgradeCategoryDA> Category);
	
	void UpdateUpgradeButtons();
	
	void OnUpgradeSelected(TObjectPtr<UShipUpgradeDA> UpgradeDA);
	void SetCameraAsset(UCameraAsset* CameraAsset);
	void RestoreGameplayCamera();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	
	UFUNCTION()
	void OnCloseButtonClicked();
	
	void Hide();
};
