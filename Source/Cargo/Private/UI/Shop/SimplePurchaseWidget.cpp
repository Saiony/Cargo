// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Shop/SimplePurchaseWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"

void USimplePurchaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PurchaseButton->OnClicked.AddDynamic(this, &ThisClass::OnPurchaseButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void USimplePurchaseWidget::OnPurchaseButtonClicked()
{
	if (ACargoGameMode::Get(this)->EconomyService->RemoveMoney(Price))
	{
		if (PurchaseCallback)
		{
			PurchaseCallback(true);
		}
		Hide();
	}
}

void USimplePurchaseWidget::OnExitButtonClicked()
{
	if (PurchaseCallback)
	{
		PurchaseCallback(false);
	}
	Hide();	
}

void USimplePurchaseWidget::Initialize(int32 Price, const FString& Description, TFunction<void(bool)> PurchaseCallback)
{
	this->PurchaseCallback = MoveTemp(PurchaseCallback);
	this->Price = Price;
	
	DescriptionText->SetText(FText::FromString(Description));
	PriceText->SetText(FText::AsNumber(Price));
	
	const auto CanPurchase = ACargoGameMode::Get(this)->EconomyService->HasMoney(Price);	
	PurchaseButton->SetIsEnabled(CanPurchase);
}

void USimplePurchaseWidget::Hide()
{	
	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}
