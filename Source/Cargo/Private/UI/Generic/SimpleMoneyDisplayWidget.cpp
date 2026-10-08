// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Generic/SimpleMoneyDisplayWidget.h"

#include "CommonTextBlock.h"

void USimpleMoneyDisplayWidget::Initialize(const int32 Price) const
{
	PriceText->SetText(FText::AsNumber(Price));
}
