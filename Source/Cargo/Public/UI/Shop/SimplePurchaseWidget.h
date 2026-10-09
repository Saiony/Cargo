// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Button.h"
#include "Templates/Function.h"
#include "SimplePurchaseWidget.generated.h"

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
	
	TFunction<void(bool)> PurchaseCallback;
	
	void Hide();
public:
	void Initialize(int32 Price, const FString& Description, TFunction<void(bool)> PurchaseCallback);
};
