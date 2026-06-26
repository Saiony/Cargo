// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestData.h"
#include "CargoGameMode.generated.h"

class APlaceable;
class UQuestStatus;

using FActiveQuestsMap = TMap<FGameplayTag, TObjectPtr<UQuestStatus>>;

DECLARE_MULTICAST_DELEGATE_OneParam(FActiveQuestsDelegate, const FActiveQuestsMap&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FQuestAcceptedDelegate, TObjectPtr<UQuestData>, AActor*);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnQuestProgressUpdatedDelegate, TObjectPtr<UQuestStatus>, FGameplayTag, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FQuestCompletedDelegate, TObjectPtr<UQuestStatus>);
	

UCLASS(abstract)
class ACargoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UQuestStatus>> ActiveQuests;
	
	int QuestFinishedDelegate;

	void CheckIfQuestEnded(TObjectPtr<UQuestStatus> QuestStatus);
public:	
	ACargoGameMode();
	
	FActiveQuestsDelegate ActiveQuestsDelegate;
	
	FQuestAcceptedDelegate QuestAcceptedDelegate;

	FOnQuestProgressUpdatedDelegate OnQuestProgressUpdatedDelegate;
	
	FQuestCompletedDelegate QuestCompletedDelegate;
	
	static ACargoGameMode* Get(const UObject* WorldContextObject)
	{
		return Cast<ACargoGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	};
	
	void AddQuest(UQuestData* QuestData, AActor* QuestInstigator = nullptr);

	TObjectPtr<UQuestStatus> GetQuestStatus(FGameplayTag QuestTag);
	TObjectPtr<UQuestStatus> GetQuestStatusByDestination(FGameplayTag Destination);

	void RegisterCargoDelivery(FGameplayTag QuestTag, FGameplayTag CargoType);
	void RemoveCargoDelivery(FGameplayTag QuestTag, FGameplayTag CargoType);
};
