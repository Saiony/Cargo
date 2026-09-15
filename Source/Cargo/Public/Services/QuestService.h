#pragma once

#include "CoreMinimal.h"
#include "FORGServiceBase.h"
#include "Mission/DeliveryMissionData.h"
#include "Mission/DeliveryMissionStatus.h"
#include "QuestService.generated.h"

enum class ShipCollisionType;

using FActiveMissionsMap = TMap<FGuid, TObjectPtr<UDeliveryMissionStatus>>;
using FActiveQuestsMap = TMap<FGameplayTag, TObjectPtr<UQuestStatus>>;

DECLARE_MULTICAST_DELEGATE_OneParam(FActiveMissionsUpdatedDelegate, const FActiveMissionsMap&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMissionAcceptedDelegate, TObjectPtr<UDeliveryMissionStatus>, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMissionProgressUpdatedDelegate, TObjectPtr<UDeliveryMissionStatus>, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_OneParam(FMissionCompletedDelegate, TObjectPtr<UDeliveryMissionStatus>);
DECLARE_MULTICAST_DELEGATE_OneParam(FActiveQuestsDelegate, const FActiveQuestsMap&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FQuestAcceptedDelegate, TObjectPtr<UQuestData>, AActor*);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnQuestProgressUpdatedDelegate, TObjectPtr<UQuestStatus>, FGameplayTag, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FQuestCompletedDelegate, TObjectPtr<UQuestStatus>);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UQuestService : public UFORGServiceBase
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FGuid, TObjectPtr<UDeliveryMissionStatus>> ActiveMissions;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UQuestStatus>> ActiveQuests;

	UPROPERTY()
	TArray<TObjectPtr<UQuestData>> AvailableQuests;

	UDeliveryMissionStatus* FindMission(FGuid MissionId) const;
	UQuestStatus* FindQuestForMission(FGuid MissionId) const;
	void UpdateCargoDelivery(FGuid MissionId, FGameplayTag CargoType, int32 Quantity);
	void OnShipCollision(AActor* OtherActor, ShipCollisionType CollisionType);

public:
	FActiveMissionsUpdatedDelegate ActiveMissionsUpdatedDelegate;
	FMissionAcceptedDelegate MissionAcceptedDelegate;
	FOnMissionProgressUpdatedDelegate MissionProgressUpdatedDelegate;
	FMissionCompletedDelegate MissionCompletedDelegate;
	FActiveQuestsDelegate ActiveQuestsDelegate;
	FQuestAcceptedDelegate QuestAcceptedDelegate;
	FOnQuestProgressUpdatedDelegate OnQuestProgressUpdatedDelegate;
	FQuestCompletedDelegate QuestCompletedDelegate;

	UQuestService();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void AcceptMission(TObjectPtr<UDeliveryMissionData> MissionData, FGameplayTag StartIslandTag);
	void RegisterCargoDelivery(FGuid MissionId, FGameplayTag CargoType);
	void RemoveCargoDelivery(FGuid MissionId, FGameplayTag CargoType);
	void CompleteMission(FGuid MissionId, AActor* InstigatorIsland);
	TArray<TObjectPtr<UDeliveryMissionStatus>> GetActiveMissionsForDestination(FGameplayTag DestinationTag);

	void ActivateQuest(UQuestData* QuestData, AActor* QuestInstigator);
	void CompleteTravelQuest(FGameplayTag QuestTag, AActor* InstigatorIsland);
	void AddAvailableQuest(TObjectPtr<UQuestData> Quest);
	TArray<TObjectPtr<UQuestStatus>> GetQuestStatus(FGameplayTag QuestTag);
	TArray<TObjectPtr<UQuestStatus>> GetQuestStatusByDestination(FGameplayTag Destination);
	TArray<TObjectPtr<UQuestStatus>> GetQuestsStatusByOrigin(FGameplayTag OriginIsland);
	TArray<TObjectPtr<UQuestData>> GetAvailableQuestsByStartLocation(FGameplayTag StartLocation);
};
