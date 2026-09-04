// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/MissionsService.h"

#include "CargoGameMode.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "Mission/MissionReward.h"


UMissionsService::UMissionsService()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMissionsService::Boot(FOnServiceBooted OnBootFinished)
{	
	Super::Boot(OnBootFinished);
}

void UMissionsService::PostLogin(APlayerController* NewPlayer)
{
	const auto PlayerState = NewPlayer->GetPlayerState<ACargoPlayerState>();
	PlayerState->OnShipCollisionEvent.AddUObject(this, &ThisClass::OnShipCollision);
}

void UMissionsService::Logout(AController* Exiting)
{
	const auto PlayerState = Exiting->GetPlayerState<ACargoPlayerState>();
	PlayerState->OnShipCollisionEvent.RemoveAll(this);
}

void UMissionsService::OnShipCollision(AActor* OtherActor, ShipCollisionType CollisionType)
{
	for (const auto ActiveMission : ActiveMissions)
	{
		if (CollisionType == ShipCollisionType::Light)
			ActiveMission.Value->AddCollision_Light();
		else if (CollisionType == ShipCollisionType::Heavy)
			ActiveMission.Value->AddCollision_Hard();
	}
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

void UMissionsService::CompleteMission(const FGuid MissionId)
{
	const TObjectPtr<UMissionStatus>* Mission = ActiveMissions.Find(MissionId);
	if (!Mission || !IsValid(*Mission))
	{
		UE_LOG(LogTemp, Warning, TEXT("MissionsService: Cannot complete inactive mission %s"), *MissionId.ToString());
		return;
	}

	CompleteMission(*Mission);
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
	const FMissionReward MissionReward = Mission->CompleteMission();
	ACargoGameMode::Get(this)->EconomyService->AddMoney(MissionReward.FinalReward.Money);
	
	MissionCompletedDelegate.Broadcast(Mission);
	ActiveMissions.Remove(Mission->GetId());
}
