// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShipWeightWidget.h"
#include "CargoCharacter.h"

void UShipWeightWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (const auto Character = GetOwningPlayerPawn<ACargoCharacter>())
	{
		Character->OnWeightChanged.AddDynamic(this, &ThisClass::HandleWeightChanged);
		HandleWeightChanged(Character->GetCurrentWeight(), Character->GetMaxWeight());
	}
}

void UShipWeightWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (const auto Character = GetOwningPlayerPawn<ACargoCharacter>())
		Character->OnWeightChanged.RemoveDynamic(this, &ThisClass::HandleWeightChanged);
}

void UShipWeightWidget::HandleWeightChanged(const float NewCurrentWeight, const float MaxWeight)
{
	UE_LOG(LogTemp, Log, TEXT("Weight changed to %f/%f"), NewCurrentWeight, MaxWeight);
	
	const auto Percent = NewCurrentWeight / MaxWeight;
	
	if (Percent >= 0.99f)
	{
		BackgroundImage->SetBrushTintColor(SuperHeavyColor);
	}
	else if (Percent >= 0.75f)
	{
		BackgroundImage->SetBrushTintColor(HeavyColor);
	}
	else if (Percent >= 0.50f)
	{
		BackgroundImage->SetBrushTintColor(NormalColor);
	}
	else if (Percent >= 0.25f)
	{
		BackgroundImage->SetBrushTintColor(LightColor);
	}
	else
	{
		BackgroundImage->SetBrushTintColor(SuperLightColor);
	}

	CurrentWeightText->SetText(FText::AsNumber(NewCurrentWeight));
	MaxWeightText->SetText(FText::AsNumber(MaxWeight));
}
