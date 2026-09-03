// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/CargoPlayerState.h"

void ACargoPlayerState::CalculateShipSpeedMultiplier()
{
	const auto Percent = CurrentWeight / MaxWeight;
	if (Percent >= 0.99f)
	{
		ShipSpeedMultiplier = 0.1f;
	}
	else if (Percent >= 0.75f)
	{
		ShipSpeedMultiplier = 0.5f;
	}
	else if (Percent >= 0.50f)
	{
		ShipSpeedMultiplier = 0.75f;
	}
	else if (Percent >= 0.25f)
	{
		ShipSpeedMultiplier = 0.8f;
	}
	else
	{
		ShipSpeedMultiplier = 1.0f;
	}
}

void ACargoPlayerState::AddWeight(float Weight)
{
	CurrentWeight += Weight;
	CalculateShipSpeedMultiplier();
	
	OnWeightChanged.Broadcast(CurrentWeight, MaxWeight);
}

void ACargoPlayerState::RemoveWeight(float Weight)
{
	CurrentWeight -= Weight;
	CalculateShipSpeedMultiplier();
	
	OnWeightChanged.Broadcast(CurrentWeight, MaxWeight);
}

void ACargoPlayerState::SetMaxWeight(float NewMaxWeight)
{
	MaxWeight = NewMaxWeight;
	CalculateShipSpeedMultiplier();
	
	OnWeightChanged.Broadcast(CurrentWeight, NewMaxWeight);
}

void ACargoPlayerState::SetShipBalanceWeight(float NewBalance)
{	
	ShipBalanceWeight = NewBalance;
	OnBalanceChanged.Broadcast(GetShipBalanceTotal());
}

void ACargoPlayerState::SetShipBalanceRotation(float NewBalance)
{
	ShipBalanceRotation = NewBalance;
	OnBalanceChanged.Broadcast(GetShipBalanceTotal());
}

void ACargoPlayerState::NotifyShipCollision(AActor* OtherActor, ShipCollisionType CollisionType)
{
	OnShipCollisionEvent.Broadcast(OtherActor, CollisionType);
}
