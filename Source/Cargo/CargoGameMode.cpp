// Copyright Epic Games, Inc. All Rights Reserved.

#include "CargoGameMode.h"

#include "Debug/CameraDebugCategories.h"
#include "Island/CargoIsland.h"
#include "Quest/QuestStatus.h"
#include "Subsystem/CargoUIManagerSubsystem.h"

void ACargoGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UCargoUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UCargoUIManagerSubsystem>())
	{
		UIManager->BindToMissionsService(MissionsService);
	}

	for (TObjectPtr<UQuestData> AvailableQuest : AvailableQuestsOnStart)
	{
		AddAvailableQuest(AvailableQuest);
	}
	
	GetComponents<UFORGServiceBase>(Services);
	BootService(0);
}

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
	MissionsService = ObjectInitializer.CreateDefaultSubobject<UMissionsService>(this, TEXT("MissionsService"));
	EconomyService = ObjectInitializer.CreateDefaultSubobject<UEconomyService>(this, TEXT("EconomyService"));
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

void ACargoGameMode::ActivateQuest(UQuestData* QuestData, AActor* QuestInstigator)
{
	if (!QuestData)
		return;

	const auto AvailableQuest = AvailableQuests.ContainsByPredicate([QuestData](const TObjectPtr<UQuestData>& Quest)
	{
		return Quest->QuestTag == QuestData->QuestTag;
	});
	
	if (!AvailableQuest)
	{
		UE_LOG(LogTemp, Error, TEXT("Quest %s is not available"), *QuestData->Title.ToString());
		return;
	}

	if (ActiveQuests.Contains(QuestData->QuestTag))
	{
		UE_LOG(LogTemp, Error, TEXT("Quest %s is already active"), *QuestData->Title.ToString());
		return;
	}
	
	
	auto Island = Cast<ACargoIsland>(QuestInstigator);

	if (Island->GetLocationTag() != QuestData->StartLocationTag)
	{
		UE_LOG(LogTemp, Error, TEXT("Island %s tried to start quest %s which should be started by island %s"),
		       *Island->GetLocationTag().ToString(), *QuestData->Title.ToString(),
		       *QuestData->StartLocationTag.ToString());

		return;
	}
	
	UQuestStatus* NewStatus = NewObject<UQuestStatus>(this);
	NewStatus->Initialize(QuestData);
	ActiveQuests.Add(QuestData->QuestTag, NewStatus);
	AvailableQuests.Remove(QuestData);

	ActiveQuestsDelegate.Broadcast(ActiveQuests);
	QuestAcceptedDelegate.Broadcast(QuestData, QuestInstigator);
}

TObjectPtr<UQuestStatus> ACargoGameMode::GetQuestStatus(FGameplayTag QuestTag)
{
	if (TObjectPtr<UQuestStatus>* Found = ActiveQuests.Find(QuestTag))
	{
		return *Found;
	}
	return nullptr;
}

TObjectPtr<UQuestStatus> ACargoGameMode::GetQuestStatusByDestination(FGameplayTag Destination)
{
	for (const auto& [Tag, Status] : ActiveQuests)
	{
		if (Status && Status->DestinationTag == Destination)
		{
			return Status;
		}
	}
	return nullptr;
}

TObjectPtr<UQuestStatus> ACargoGameMode::GetQuestStatusByOrigin(FGameplayTag OriginIsland)
{
	for (const auto& [Tag, Status] : ActiveQuests)
	{
		if (Status && Status->StartIslandTag == OriginIsland)
		{
			return Status;
		}
	}

	return nullptr;
}

void ACargoGameMode::RegisterCargoDelivery(FGameplayTag QuestTag, FGameplayTag CargoType)
{
	TObjectPtr<UQuestStatus> QuestStatus = GetQuestStatus(QuestTag);
	if (!QuestStatus)
		return;

	auto& CargoStatus = QuestStatus->DeliveredQuantities.FindOrAdd(CargoType);
	CargoStatus.DeliveredQuantity++;

	OnQuestProgressUpdatedDelegate.Broadcast(QuestStatus, CargoType, CargoStatus.DeliveredQuantity);

	CheckIfQuestEnded(QuestStatus);
}

void ACargoGameMode::RemoveCargoDelivery(FGameplayTag QuestTag, FGameplayTag CargoType)
{
	TObjectPtr<UQuestStatus> QuestStatus = GetQuestStatus(QuestTag);
	if (!QuestStatus)
		return;

	auto& CargoStatus = QuestStatus->DeliveredQuantities[CargoType];
	CargoStatus.DeliveredQuantity--;

	OnQuestProgressUpdatedDelegate.Broadcast(QuestStatus, CargoType, CargoStatus.DeliveredQuantity);
}

void ACargoGameMode::AddTag(FGameplayTag ChoiceTag)
{
	TagsContainer.AddTag(ChoiceTag);
}

bool ACargoGameMode::HasTag(FGameplayTag ChoiceName)
{
	return TagsContainer.HasTag(ChoiceName);
}

TObjectPtr<UQuestData> ACargoGameMode::GetAvailableQuestByStartLocation(FGameplayTag StartLocation)
{
	const TObjectPtr<UQuestData>* Found = AvailableQuests.FindByPredicate([StartLocation](TObjectPtr<UQuestData> QuestData)
	{
		return QuestData->StartLocationTag == StartLocation;
	});
	
	return Found ? *Found : nullptr;
}

void ACargoGameMode::CheckIfQuestEnded(TObjectPtr<UQuestStatus> QuestStatus)
{
	for (auto Delivered : QuestStatus->DeliveredQuantities)
	{
		if (!Delivered.Value.IsComplete())
			return;
	}

	UE_LOG(LogTemp, Log, TEXT("Quest %s ended"), *QuestStatus->OriginalQuestData->Title.ToString());
	
	//give reward
	if (QuestStatus->Reward.RewardTag.IsValid())
		AddTag(QuestStatus->Reward.RewardTag);
	
	EconomyService->AddMoney(QuestStatus->Reward.Money);

	//handle quest lifecycle/events
	ActiveQuests.Remove(QuestStatus->OriginalQuestData->QuestTag);
	AddAvailableQuest(QuestStatus->NextQuest.LoadSynchronous());
	QuestCompletedDelegate.Broadcast(QuestStatus);
}

void ACargoGameMode::AddAvailableQuest(TObjectPtr<UQuestData> Quest)
{
	if (Quest == nullptr)
		return;
	
	AvailableQuests.Add(Quest);
}
