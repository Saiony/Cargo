// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DryDock/UpgradeInfoSingle.h"

void UUpgradeInfoSingle::Initialize(const FText& Name, UTexture2D* Icon, float Value)
{
	AttributeIcon->SetBrushFromTexture(Icon);
	AttributeName->SetText(Name);
	AttributeValue->SetText(FText::AsNumber(Value));
}
