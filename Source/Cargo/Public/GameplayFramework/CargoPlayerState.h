// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "GameplayFramework/Domains/FuelDomain.h"
#include "CargoPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBaseSpeedChanged, float, NewBaseSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShipNameChanged, FString, NewShipName);

enum class ShipCollisionType
{
	Unknown = 0,
	Light,
	Heavy
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnShipCollision, AActor*, ShipCollisionType);

/**
 * 
 */
UCLASS()
class CARGO_API ACargoPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	FFuelDomain FuelDomain{100.f, 100.f};

	TMap<FGameplayTag, FString> PlayerDataTags;
	
public:
	// --- Events ---

	UPROPERTY(BlueprintAssignable, Category = "Cargo")
	FOnBaseSpeedChanged OnBaseSpeedChanged;

	FOnShipCollision OnShipCollisionEvent;
	
	FOnShipNameChanged OnShipNameChanged;
	
	// --- Getters ---

	FFuelDomain* GetFuelDomain() { return &FuelDomain; }

	FString GetShipName() const { return PlayerDataTags.FindRef(FGameplayTag::RequestGameplayTag(TEXT("PlayerData.ShipName"))); }
	
	FString GetCaptainName() const { return PlayerDataTags.FindRef(FGameplayTag::RequestGameplayTag(TEXT("PlayerData.CaptainName"))); }
	
	FString GetPlayerInputText(FGameplayTag Tag) const;

	const FString* FindPlayerDataTag(FGameplayTag Tag) const { return PlayerDataTags.Find(Tag); }

	// --- Setters ---

	void NotifyShipCollision(AActor* OtherActor, ShipCollisionType CollisionType);
	
	void AddPlayerDataTag(FGameplayTag Tag, FString Text);
};
