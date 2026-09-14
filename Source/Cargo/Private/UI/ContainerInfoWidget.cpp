// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ContainerInfoWidget.h"

void UContainerInfoWidget::Show(const FText& Cargo, float Weight)
{	
	CargoText->SetText(Cargo);
	WeightText->SetText(FText::AsNumber(Weight));
}

void UContainerInfoWidget::Hide()
{
}
