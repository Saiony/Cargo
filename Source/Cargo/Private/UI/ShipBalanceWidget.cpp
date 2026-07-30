// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShipBalanceWidget.h"
#include "GameplayFramework/CargoPlayerState.h"

void UShipBalanceWidget::NativeConstruct()
{
	Super::NativeConstruct();
	const auto PlayerState = GetOwningPlayerState<ACargoPlayerState>();
	PlayerState->OnBalanceChanged.AddDynamic(this, &ThisClass::HandleBalanceChanged);
}

void UShipBalanceWidget::NativeDestruct()
{
	Super::NativeDestruct();
	const auto PlayerState = GetOwningPlayerState<ACargoPlayerState>();
	
	if (PlayerState) //when game closes suddenly, player state is destroyed previously
		PlayerState->OnBalanceChanged.RemoveDynamic(this, &ThisClass::HandleBalanceChanged);
}

void UShipBalanceWidget::HandleBalanceChanged(float NewBalance)
{
	UE_LOG(LogTemp, Log, TEXT("Balance changed to %f"), NewBalance);
    NeedleImg->SetRenderTransformAngle(-NewBalance); //ui rotates clockwise
}
