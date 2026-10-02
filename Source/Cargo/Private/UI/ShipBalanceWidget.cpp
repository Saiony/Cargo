// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShipBalanceWidget.h"
#include "CargoCharacter.h"

void UShipBalanceWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (const auto Character = GetOwningPlayerPawn<ACargoCharacter>())
	{
		Character->OnBalanceChanged.AddDynamic(this, &ThisClass::HandleBalanceChanged);
		HandleBalanceChanged(Character->GetCurrentShipRoll());
	}
}

void UShipBalanceWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (const auto Character = GetOwningPlayerPawn<ACargoCharacter>())
		Character->OnBalanceChanged.RemoveDynamic(this, &ThisClass::HandleBalanceChanged);
}

void UShipBalanceWidget::HandleBalanceChanged(float NewBalance)
{
	UE_LOG(LogTemp, Log, TEXT("Balance changed to %f"), NewBalance);
    NeedleImg->SetRenderTransformAngle(-NewBalance); //ui rotates clockwise
}
