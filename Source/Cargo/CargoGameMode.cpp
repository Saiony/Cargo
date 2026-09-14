// Copyright Epic Games, Inc. All Rights Reserved.

#include "CargoGameMode.h"

#include "Debug/CameraDebugCategories.h"

void ACargoGameMode::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	AddDebugInitialTags();
#endif

	for (TObjectPtr<UQuestData> AvailableQuest : AvailableQuestsOnStart)
	{
		QuestService->AddAvailableQuest(AvailableQuest);
	}
	
	GetComponents<UFORGServiceBase>(Services);
	BootService(0);
}

#if WITH_EDITOR
void ACargoGameMode::AddDebugInitialTags()
{
	for (const FGameplayTag& DebugTag : Debug_InitialTags)
	{
		AddTag(DebugTag);
	}
}
#endif

void ACargoGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (Services.IsEmpty())
		GetComponents<UFORGServiceBase>(Services);

	for (const auto Service : Services)
		Service->PostLogin(NewPlayer);
}

void ACargoGameMode::Logout(AController* Exiting)
{
	for (const auto Service : Services)
		Service->Logout(Exiting);

	Super::Logout(Exiting);
}

ACargoGameMode::ACargoGameMode(const FObjectInitializer& ObjectInitializer)
{
	// Keep the serialized component name for existing Blueprints.
	QuestService = ObjectInitializer.CreateDefaultSubobject<UQuestService>(this, TEXT("MissionsService"));
	EconomyService = ObjectInitializer.CreateDefaultSubobject<UEconomyService>(this, TEXT("EconomyService"));
	UIService = ObjectInitializer.CreateDefaultSubobject<UUIService>(this, TEXT("UIService"));
		
	ServicesMap.Add(QuestService->GetClass(), QuestService);
	ServicesMap.Add(EconomyService->GetClass(), EconomyService);
	ServicesMap.Add(UIService->GetClass(), UIService);
}

void ACargoGameMode::BootService(const int32 Index)
{
	if(Index > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("%s booted successfully"), *Services[Index-1]->GetName());
	}

	if(!Services.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Log, TEXT("All Services booted successfully"));

		OnServicesBooted.Broadcast();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Booting %s..."), *Services[Index]->GetName());
	Services[Index]->Boot(FOnServiceBooted::CreateUObject(this, &ACargoGameMode::BootService, Index + 1));
}

void ACargoGameMode::AddTag(FGameplayTag ChoiceTag)
{
	TagsContainer.AddTag(ChoiceTag);
}

bool ACargoGameMode::HasTag(FGameplayTag ChoiceName)
{
	return TagsContainer.HasTag(ChoiceName);
}

