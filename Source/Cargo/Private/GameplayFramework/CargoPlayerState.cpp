// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/CargoPlayerState.h"

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
	
	PlayerDataTags.Add(Tag, Text);
	
	
	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("PlayerData.ShipName"), false)))
		OnShipNameChanged.Broadcast(Text);	
}
