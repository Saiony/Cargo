// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MissionData.h"
#include "UObject/Object.h"
#include "../Quest/QuestStatus.h"
#include "MissionStatus.generated.h"

struct FCargoStatus;
/**
 * 
 */
UCLASS()
class CARGO_API UMissionStatus : public UObject
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
	TObjectPtr<UMissionData> OriginalMissionData;

public:
	void Initialize(TObjectPtr<UMissionData> MissionData, FGameplayTag InStartIslandTag);
	
	void AddCargo(FGameplayTag CargoType, int32 Quantity);
	
	void RemoveCargo(FGameplayTag CargoType, int32 Quantity);
	
	FGameplayTag GetStartIslandTag() const { return StartIslandTag; }
	
	FGameplayTag GetDestinationTag() const { return DestinationTag; }
	
	TObjectPtr<UMissionData> GetOriginalMissionData() const { return OriginalMissionData; }
	
	FGuid GetId() const { return Id; }
	
	void AddCargoDelivery(FGameplayTag CargoType);
	
	void RemoveCargoDelivery(FGameplayTag CargoType);
	
	bool IsComplete() const;
	
	FReward Reward;
	
	TMap<FGameplayTag, FCargoStatus> GetDeliveredQuantities() const { return DeliveredQuantities; }
};
