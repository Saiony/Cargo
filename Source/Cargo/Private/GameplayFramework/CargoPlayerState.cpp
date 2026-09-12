// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/CargoPlayerState.h"

void ACargoPlayerState::CalculateShipSpeedMultiplier()
{
	const auto Percent = CurrentWeight / MaxWeight;
	if (Percent >= 0.99f)
	{
		ShipSpeedMultiplier = 0.25f;
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

FString ACargoPlayerState::GetPlayerInputText(FGameplayTag Tag) const
{
	const auto Text = PlayerDataTags.Find(Tag);
	
	if (!Text)
	{
		UE_LOG(LogTemp, Error, TEXT("ACargoPlayerState::GetPlayerInputText: Invalid tag '%s'"), *Tag.ToString());
		return FString();
	}
	
	return *Text;
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

void ACargoPlayerState::AddPlayerDataTag(FGameplayTag Tag, FString Text)
{
	if (!Tag.MatchesTag(Tag))
	{
		UE_LOG(LogTemp, Error, TEXT("ACargoPlayerState::AddPlayerInputText: Invalid tag '%s'"), *Tag.ToString());
		return;
	}
	
	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("PlayerData.ShipName"), false)))
		OnShipNameChanged.Broadcast(Text);
	
	PlayerDataTags.Add(Tag, Text);
}
