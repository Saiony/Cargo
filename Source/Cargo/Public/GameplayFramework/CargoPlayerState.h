// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CargoPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeightChanged, float, NewCurrentWeight, float, MaxWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBaseSpeedChanged, float, NewBaseSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBalanceChanged, float, NewBalance);

/**
 * 
 */
UCLASS()
class CARGO_API ACargoPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float CurrentWeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float MaxWeight = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float ShipBalance = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
	float ShipSpeedMultiplier = 1;
	
	void CalculateShipSpeedMultiplier();
	
public:	
	// --- Events ---

	UPROPERTY(BlueprintAssignable, Category = "Cargo")
	FOnWeightChanged OnWeightChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Cargo")
	FOnBaseSpeedChanged OnBaseSpeedChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Cargo")
	FOnBalanceChanged OnBalanceChanged;
	
	// --- Getters ---

	UFUNCTION(BlueprintPure, Category = "Cargo")
	float GetCurrentWeight() const { return CurrentWeight; }

	UFUNCTION(BlueprintPure, Category = "Cargo")
	float GetMaxWeight() const { return MaxWeight; }
	
	UFUNCTION(BlueprintPure, Category = "Cargo")
	float GetShipSpeedMultiplier() const { return ShipSpeedMultiplier; }

	// --- Setters ---

	UFUNCTION(Category = "Cargo")
	void AddWeight(float Weight);
	
	UFUNCTION(Category = "Cargo")
	void RemoveWeight(float Weight);	

	UFUNCTION(Category = "Cargo")
	void SetMaxWeight(float NewMaxWeight);
	
	UFUNCTION(Category = "Cargo")
	void SetShipBalance(float NewBalance);
};
