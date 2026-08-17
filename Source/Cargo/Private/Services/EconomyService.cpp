// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/EconomyService.h"


// Sets default values for this component's properties
UEconomyService::UEconomyService()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UEconomyService::HasMoney(const int32 Amount)
{
	return Money >= Amount;
}

bool UEconomyService::RemoveMoney(const int32 Amount)
{
	if (!HasMoney(Amount))
		return false;
	
	Money -= Amount;
	OnMoneyChanged.Broadcast(Money);
	
	return true;
}

void UEconomyService::AddMoney(int32 Amount)
{
	if (Amount <= 0)
		return;
	
	Money += Amount;	
	OnMoneyChanged.Broadcast(Money);
}
