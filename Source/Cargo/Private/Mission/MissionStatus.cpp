// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission/MissionStatus.h"

#include "Mission/MissionReward.h"
#include "Quest/QuestData.h"
#include "Quest/QuestStatus.h"

void UMissionStatus::Initialize(TObjectPtr<UMissionData> MissionData, FGameplayTag InStartIslandTag)
{
	Id = MissionData->GetId();
	OriginalMissionData = MissionData;
	StartIslandTag = InStartIslandTag;
	DestinationTag = MissionData->DestinationTag;	
	BaseReward = MissionData->Reward;
	NumShipCollisions_Light = 0;
	NumShipCollisions_Hard = 0;
	
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

int32 UMissionStatus::GetNumDamagedContainers() const
{
	int32 NumDamaged = 0;
	for (auto Delivered : DeliveredQuantities)
	{
		if (Delivered.Value.IsDamaged)
			NumDamaged++;
	}
	
	return NumDamaged;
}

FMissionReward UMissionStatus::CompleteMission()
{
	IsCompleted = true;
	return FMissionReward(this);
}

void UMissionStatus::AddCollision_Light()
{
	NumShipCollisions_Light++;
}

void UMissionStatus::AddCollision_Hard()
{
	NumShipCollisions_Hard++;
}
