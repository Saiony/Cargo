// Copyright Epic Games, Inc. All Rights Reserved.

#include "CargoGameMode.h"

#include "Island/CargoIsland.h"
#include "Quest/QuestStatus.h"

void ACargoGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	AddAvailableQuest(FirstQuest);
}

ACargoGameMode::ACargoGameMode()
{
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

void ACargoGameMode::AddChoice(FGameplayTag ChoiceTag)
{
	ChoicesContainer.AddTag(ChoiceTag);
}

bool ACargoGameMode::HasChoice(FGameplayTag ChoiceName)
{
	return ChoicesContainer.HasTag(ChoiceName);
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
