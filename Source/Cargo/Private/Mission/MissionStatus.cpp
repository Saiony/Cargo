// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission/MissionStatus.h"

#include "Quest/QuestData.h"
#include "Quest/QuestStatus.h"

void UMissionStatus::Initialize(TObjectPtr<UMissionData> MissionData, FGameplayTag InStartIslandTag)
{
	Id = MissionData->GetId();
	OriginalMissionData = MissionData;
	StartIslandTag = InStartIslandTag;
	DestinationTag = MissionData->DestinationTag;	
	Reward = MissionData->Reward;
	
	for (const auto Req : MissionData->CargoRequirements)
	{
		auto CargoStatus =  FCargoStatus(Req.CargoType, 0, Req.Quantity);
		DeliveredQuantities.Add(Req.CargoType, CargoStatus);
	}
}

void UMissionStatus::AddCargo(FGameplayTag CargoType, int32 Quantity)
{
	if (!DeliveredQuantities.Contains(CargoType))
	{
		UE_LOG(LogTemp, Log, TEXT("CargoStatus: Added cargo irrelevant for mission - %s"), *CargoType.ToString())
		return;
	}
	
	DeliveredQuantities[CargoType].DeliveredQuantity += Quantity;
}

void UMissionStatus::RemoveCargo(FGameplayTag CargoType, int32 Quantity)
{
	if (!DeliveredQuantities.Contains(CargoType))
	{
		UE_LOG(LogTemp, Log, TEXT("CargoStatus: Removed cargo irrelevant for mission - %s"), *CargoType.ToString())
		return;
	}
	
	DeliveredQuantities[CargoType].DeliveredQuantity -= Quantity;
}

void UMissionStatus::AddCargoDelivery(FGameplayTag CargoType)
{
	AddCargo(CargoType, 1);
}

void UMissionStatus::RemoveCargoDelivery(FGameplayTag CargoType)
{
	RemoveCargo(CargoType, 1);
}

bool UMissionStatus::IsComplete() const
{
	for (auto Delivered : DeliveredQuantities)
	{
		if (!Delivered.Value.IsComplete())
			return false;
	}
	
	return true;
}
