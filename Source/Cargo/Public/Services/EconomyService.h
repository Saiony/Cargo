// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FORGServiceBase.h"
#include "EconomyService.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, int32)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UEconomyService : public UFORGServiceBase
{
	GENERATED_BODY()
	
	int32 Money = 0.0f;
	int32 FuelDebt = 0;	
	int32 FuelDebtPrice = 25;
	
public:	
	FOnMoneyChanged OnMoneyChanged;
	
	UEconomyService();
	
	/*money*/
	int32 GetMoney() const { return Money; }	
	bool HasMoney(int32 Amount);	
	bool RemoveMoney(int32 Amount);	
	void AddMoney(int32 Amount);	
	
	/*fuel*/
	int32 GetFuelDebt() const { return FuelDebt; };
	void IncrementFuelDebt();
	void PayFuelDebts();
};
