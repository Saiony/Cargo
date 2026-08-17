// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/MissionsService.h"

#include "CargoGameMode.h"


UMissionsService::UMissionsService()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMissionsService::Boot(FOnServiceBooted OnBootFinished)
{	
	Super::Boot(OnBootFinished);
}

void UMissionsService::AcceptMission(const TObjectPtr<UMissionData> MissionData, FGameplayTag StartIslandTag)
{
	UMissionStatus* NewMission = NewObject<UMissionStatus>(this);
	NewMission->Initialize(MissionData, StartIslandTag);
	
	ActiveMissions.Add(MissionData->GetId(), NewMission);
	ActiveMissionsUpdatedDelegate.Broadcast(ActiveMissions);
	MissionAcceptedDelegate.Broadcast(NewMission, StartIslandTag);
}

void UMissionsService::RegisterCargoDelivery(const FGuid MissionId, FGameplayTag CargoType)
{
	const auto Mission = *ActiveMissions.Find(MissionId);
	if (!Mission)
	{
		UE_LOG(LogTemp, Error, TEXT("MissionsService: No active mission found for id %s"), *MissionId.ToString())
		return;
	}
	
	Mission->AddCargoDelivery(CargoType);
	MissionProgressUpdatedDelegate.Broadcast(Mission, CargoType);
	
	if (Mission->IsComplete())
		CompleteMission(Mission);
}

void UMissionsService::RemoveCargoDelivery(const FGuid MissionId, FGameplayTag CargoType)
{
	const auto Mission = ActiveMissions.Find(MissionId);
	if (!Mission)
	{
		UE_LOG(LogTemp, Error, TEXT("MissionsService: No active mission found for id %s"), *MissionId.ToString())
		return;
	}
	
	(*Mission)->RemoveCargoDelivery(CargoType);
	MissionProgressUpdatedDelegate.Broadcast(*Mission, CargoType);
}

TArray<TObjectPtr<UMissionStatus>> UMissionsService::GetActiveMissionsForDestination(const FGameplayTag DestinationTag)
{
	TArray<TObjectPtr<UMissionStatus>> Missions = TArray<TObjectPtr<UMissionStatus>>();

	for (const auto ActiveMission : ActiveMissions)
	{
		if (ActiveMission.Value->GetDestinationTag() == DestinationTag)
		{
			Missions.Add(ActiveMission.Value);
		}
	}
	
	return Missions;
}

void UMissionsService::CompleteMission(const TObjectPtr<UMissionStatus> Mission)
{
	ACargoGameMode::Get(this)->EconomyService->AddMoney(Mission->Reward.Money);
	
	MissionCompletedDelegate.Broadcast(Mission);
	ActiveMissions.Remove(Mission->GetId());
}
