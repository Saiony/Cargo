// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Money/MoneyWidget.h"

#include "CargoGameMode.h"
#include "CommonTextBlock.h"

void UMoneyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ACargoGameMode::Get(this)->EconomyService->OnMoneyChanged.AddUObject(this, &ThisClass::OnMoneyChanged);
	OnMoneyChanged(ACargoGameMode::Get(this)->EconomyService->GetMoney());
}

void UMoneyWidget::OnMoneyChanged(int32 Amount)
{
	MoneyText->SetText(FText::AsNumber((Amount)));	
}
