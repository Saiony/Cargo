// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMissionStatus.h"
#include "GameplayTagContainer.h"
#include "DeliveryMissionData.h"
#include "UObject/Object.h"
#include "../Quest/QuestStatus.h"
#include "DeliveryMissionStatus.generated.h"

struct FMissionReward;

/**
 * 
 */
UCLASS()
class CARGO_API UDeliveryMissionStatus : public UBaseMissionStatus
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	FGuid Id;
	
	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, FCargoStatus> DeliveredQuantities = TMap<FGameplayTag, FCargoStatus>();
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag StartIslandTag;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag DestinationTag;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDeliveryMissionData> OriginalMissionData;	
	
	int32 NumShipCollisions_Light = -1;
	
	int32 NumShipCollisions_Hard = -1;

	FReward BaseReward;
	
	bool IsCompleted = false;

public:
	void Initialize(TObjectPtr<UDeliveryMissionData> MissionData, FGameplayTag InStartIslandTag, FGuid InstanceId = FGuid());
	
	void AddCargo(FGameplayTag CargoType, int32 Quantity);
	
	void RemoveCargo(FGameplayTag CargoType, int32 Quantity);
	
	FGameplayTag GetStartIslandTag() const { return StartIslandTag; }
	
	FGameplayTag GetDestinationTag() const { return DestinationTag; }
	
	TObjectPtr<UDeliveryMissionData> GetOriginalMissionData() const { return OriginalMissionData; }
	
	FGuid GetId() const { return Id; }
	
	void AddCargoDelivery(FGameplayTag CargoType);
	
	void RemoveCargoDelivery(FGameplayTag CargoType);
	
	int32 GetNumDamagedContainers() const;	
	
	TMap<FGameplayTag, FCargoStatus> GetDeliveredQuantities() const { return DeliveredQuantities; }
	
	int32 GetNumShipCollisions_Light() const { return NumShipCollisions_Light; }
	
	int32 GetNumShipCollisions_Hard() const { return NumShipCollisions_Hard; }
	
	FReward GetBaseReward() const { return BaseReward; }
	
	FMissionReward CompleteMission();
	
	void AddCollision_Light();
	
	void AddCollision_Hard();
};
