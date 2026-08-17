// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FORGServiceBase.h"
#include "Mission/MissionData.h"
#include "Mission/MissionStatus.h"
#include "MissionsService.generated.h"

using FActiveMissionsMap = TMap<FGuid, TObjectPtr<UMissionStatus>>;

DECLARE_MULTICAST_DELEGATE_OneParam(FActiveMissionsUpdatedDelegate, const FActiveMissionsMap&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMissionAcceptedDelegate, TObjectPtr<UMissionStatus>, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMissionProgressUpdatedDelegate, TObjectPtr<UMissionStatus>, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_OneParam(FMissionCompletedDelegate, TObjectPtr<UMissionStatus>);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UMissionsService : public UFORGServiceBase
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<FGuid, TObjectPtr<UMissionStatus>> ActiveMissions;
		
	void CompleteMission(TObjectPtr<UMissionStatus> Mission);
	
public:
	FActiveMissionsUpdatedDelegate ActiveMissionsUpdatedDelegate;
	FMissionAcceptedDelegate MissionAcceptedDelegate;
	FOnMissionProgressUpdatedDelegate MissionProgressUpdatedDelegate;
	FMissionCompletedDelegate MissionCompletedDelegate;	
	
	// Sets default values for this component's properties
	UMissionsService();
	
	virtual void Boot(FOnServiceBooted OnBootFinished) override;
	
	void AcceptMission(TObjectPtr<UMissionData> MissionData, FGameplayTag StartIslandTag);
	
	void RegisterCargoDelivery(FGuid MissionId, FGameplayTag CargoType);
	
	void RemoveCargoDelivery(FGuid MissionId, FGameplayTag CargoType);

	TArray<TObjectPtr<UMissionStatus>> GetActiveMissionsForDestination(FGameplayTag DestinationTag);
};
