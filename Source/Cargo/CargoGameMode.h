// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestData.h"
#include "Services/EconomyService.h"
#include "Services/MissionsService.h"
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
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TArray<TObjectPtr<UQuestData>> AvailableQuestsOnStart;
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UQuestStatus>> ActiveQuests;
	
	UPROPERTY()
	TArray<TObjectPtr<UQuestData>> AvailableQuests;
	
	TInlineComponentArray<TObjectPtr<UFORGServiceBase>> Services;
	
	int QuestFinishedDelegate;	
	
	FGameplayTagContainer TagsContainer = FGameplayTagContainer();	
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	void BootService(int32 Index);
	
	//Delegates
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServicesBooted);
	UPROPERTY(BlueprintAssignable)
	FOnServicesBooted OnServicesBooted;
	
public:		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMissionsService> MissionsService;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UEconomyService> EconomyService;
	
	ACargoGameMode(const FObjectInitializer& ObjectInitializer);

	FActiveQuestsDelegate ActiveQuestsDelegate;
	
	FQuestAcceptedDelegate QuestAcceptedDelegate;

	FOnQuestProgressUpdatedDelegate OnQuestProgressUpdatedDelegate;
	
	FQuestCompletedDelegate QuestCompletedDelegate;	
	
	
	static ACargoGameMode* Get(const UObject* WorldContextObject)
	{
		return Cast<ACargoGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	};
	
	void ActivateQuest(UQuestData* QuestData, AActor* QuestInstigator = nullptr);

	TObjectPtr<UQuestStatus> GetQuestStatus(FGameplayTag QuestTag);
	TObjectPtr<UQuestStatus> GetQuestStatusByDestination(FGameplayTag Destination);
	TObjectPtr<UQuestStatus> GetQuestStatusByOrigin(FGameplayTag OriginIsland);

	void RegisterCargoDelivery(FGameplayTag QuestTag, FGameplayTag CargoType);
	void RemoveCargoDelivery(FGameplayTag QuestTag, FGameplayTag CargoType);
	
	void AddTag(FGameplayTag ChoiceTag);
	bool HasTag(FGameplayTag ChoiceName);	
	float GetGridCellSize() const { return 100.0f; }
	
	TObjectPtr<UQuestData> GetAvailableQuestByStartLocation(FGameplayTag StartLocation);
	
	void CheckIfQuestEnded(TObjectPtr<UQuestStatus> QuestStatus);	
	
	void AddAvailableQuest(TObjectPtr<UQuestData> Quest);
};
