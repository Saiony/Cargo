// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Button.h"
#include "SimplePurchaseWidget.generated.h"


UINTERFACE()
class USimplePurchaseWidgetListener : public UInterface
{
	GENERATED_BODY()
};

class ISimplePurchaseWidgetListener
{
	GENERATED_BODY()

public:
	virtual void OnSimplePurchaseWidgetCallback(bool Purchased) = 0;
};

/**
 * 
 */
UCLASS()
class CARGO_API USimplePurchaseWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DescriptionText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PriceText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> PurchaseButton;

	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void OnPurchaseButtonClicked();
	
	UFUNCTION()
	void OnExitButtonClicked();
	
	int32 Price;
	
	ISimplePurchaseWidgetListener* Listener;
	
	void Hide();
public:
	void Initialize(int32 Price, const FString& Description, ISimplePurchaseWidgetListener* Listener);
};
