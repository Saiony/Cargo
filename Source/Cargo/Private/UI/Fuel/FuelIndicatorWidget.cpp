// Fill out your copyright notice in the Description page of Project Settings.


#include "FuelIndicatorWidget.h"
#include "Components/ProgressBar.h"
#include "GameplayFramework/CargoPlayerState.h"

void UFuelIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const auto PlayerState = GetOwningPlayerState<ACargoPlayerState>();
	const auto Fuel = PlayerState->GetFuelDomain();	
	
	Fuel->OnFuelChanged.AddUObject(this, &ThisClass::OnFuelChanged);	
	OnFuelChanged(Fuel->GetCurrentFuel(), Fuel->GetMaxFuel());
}

void UFuelIndicatorWidget::NativeDestruct()
{
	const auto PlayerState = GetOwningPlayerState<ACargoPlayerState>();
	if (PlayerState)
		PlayerState->GetFuelDomain()->OnFuelChanged.RemoveAll(this);

	Super::NativeDestruct();
}

void UFuelIndicatorWidget::OnFuelChanged(float CurrentFuel, float MaxFuel)
{
	FuelBar->SetPercent(CurrentFuel / MaxFuel);
}
