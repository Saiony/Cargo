// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestData.h"
#include "Services/EconomyService.h"
#include "Services/QuestService.h"
#include "Services/UIService.h"
#include "CargoGameMode.generated.h"

class APlaceable;
class UQuestStatus;

UCLASS(abstract)
class ACargoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TArray<TObjectPtr<UQuestData>> AvailableQuestsOnStart;
	
	TInlineComponentArray<TObjectPtr<UFORGServiceBase>> Services;
	
	UPROPERTY()
	TMap<TSubclassOf<UFORGServiceBase>, TObjectPtr<UFORGServiceBase>> ServicesMap;
	
	FGameplayTagContainer InGameEventsContainer = FGameplayTagContainer();	
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	void BootService(int32 Index);

#if WITH_EDITOR
	void AddDebugInitialTags();
#endif
	
	//Delegates
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServicesBooted);
	UPROPERTY(BlueprintAssignable)
	FOnServicesBooted OnServicesBooted;
	

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category="Cargo|Debug")
	TArray<FGameplayTag> Debug_InitialTags;
#endif
	
public:		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UQuestService> QuestService;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UEconomyService> EconomyService;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UUIService> UIService;	
	
	ACargoGameMode(const FObjectInitializer& ObjectInitializer);

	static ACargoGameMode* Get(const UObject* WorldContextObject)
	{
		return Cast<ACargoGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	};
		
	void AddInGameEventTag(FGameplayTag ChoiceTag);
	bool HasInGameEventTag(FGameplayTag ChoiceName);
	
	TSet<FGuid> AlreadyPlayedDialogues;
	
	float GetGridCellSize() const { return 100.0f; }
	
	template <typename T>
	T* GetService() const
	{
		static_assert(TIsDerivedFrom<T, UFORGServiceBase>::IsDerived, "T must derive from UFORGServiceBase");

		auto Service = ServicesMap.Find(T::StaticClass());
		if (!Service)
			return nullptr;
			
		return Cast<T>(*Service);
	}
};
