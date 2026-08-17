// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ContainerInfoWidget.h"

void UContainerInfoWidget::Show(FString Cargo, float Weight)
{	
	CargoText->SetText(FText::FromString(Cargo));
	WeightText->SetText(FText::AsNumber(Weight));
}

void UContainerInfoWidget::Hide()
{
}
