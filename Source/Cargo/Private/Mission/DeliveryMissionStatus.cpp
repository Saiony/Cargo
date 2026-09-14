// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission/DeliveryMissionStatus.h"

#include "Mission/MissionReward.h"
#include "Quest/QuestData.h"
#include "Quest/QuestStatus.h"

void UDeliveryMissionStatus::Initialize(TObjectPtr<UDeliveryMissionData> MissionData, FGameplayTag InStartIslandTag, FGuid InstanceId)
{
	Id = InstanceId.IsValid() ? InstanceId : MissionData->GetId();
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

void UDeliveryMissionStatus::AddCargo(FGameplayTag CargoType, int32 Quantity)
{
	if (!DeliveredQuantities.Contains(CargoType))
	{
		UE_LOG(LogTemp, Log, TEXT("CargoStatus: Added cargo irrelevant for mission - %s"), *CargoType.ToString())
		return;
	}
	
	DeliveredQuantities[CargoType].DeliveredQuantity += Quantity;
}

void UDeliveryMissionStatus::RemoveCargo(FGameplayTag CargoType, int32 Quantity)
{
	if (!DeliveredQuantities.Contains(CargoType))
	{
		UE_LOG(LogTemp, Log, TEXT("CargoStatus: Removed cargo irrelevant for mission - %s"), *CargoType.ToString())
		return;
	}
	
	DeliveredQuantities[CargoType].DeliveredQuantity -= Quantity;
}

void UDeliveryMissionStatus::AddCargoDelivery(FGameplayTag CargoType)
{
	AddCargo(CargoType, 1);
}

void UDeliveryMissionStatus::RemoveCargoDelivery(FGameplayTag CargoType)
{
	RemoveCargo(CargoType, 1);
}

int32 UDeliveryMissionStatus::GetNumDamagedContainers() const
{
	int32 NumDamaged = 0;
	for (auto Delivered : DeliveredQuantities)
	{
		if (Delivered.Value.IsDamaged)
			NumDamaged++;
	}
	
	return NumDamaged;
}

FMissionReward UDeliveryMissionStatus::CompleteMission()
{
	IsCompleted = true;
	return FMissionReward(this);
}

void UDeliveryMissionStatus::AddCollision_Light()
{
	NumShipCollisions_Light++;
}

void UDeliveryMissionStatus::AddCollision_Hard()
{
	NumShipCollisions_Hard++;
}
