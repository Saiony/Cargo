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

public:
	FOnMoneyChanged OnMoneyChanged;
	
	UEconomyService();
	
	int32 GetMoney() const { return Money; }
	
	bool HasMoney(int32 Amount);
	
	bool RemoveMoney(int32 Amount);
	
	void AddMoney(int32 Amount);
};
