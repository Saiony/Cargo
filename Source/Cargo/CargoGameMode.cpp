// Copyright Epic Games, Inc. All Rights Reserved.

#include "CargoGameMode.h"

#include "Island/CargoIsland.h"
#include "Quest/QuestStatus.h"

ACargoGameMode::ACargoGameMode()
{
	// stub
}

void ACargoGameMode::AddQuest(UQuestData* QuestData, AActor* QuestInstigator)
{
	if (!QuestData) 
		return;
	
	if (!ActiveQuests.Contains(QuestData->QuestTag))
	{
		UQuestStatus* NewStatus = NewObject<UQuestStatus>(this);
		auto Island = Cast<ACargoIsland>(QuestInstigator);
		NewStatus->Initialize(QuestData, Island->GetLocationTag());
		ActiveQuests.Add(QuestData->QuestTag, NewStatus);
	}
	
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

void ACargoGameMode::CheckIfQuestEnded(TObjectPtr<UQuestStatus> QuestStatus)
{
	for (auto Delivered : QuestStatus->DeliveredQuantities)
	{
		if (!Delivered.Value.IsComplete())
			return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Quest %s ended"), *QuestStatus->OriginalQuestData->Title.ToString());
	
	ActiveQuests.Remove(QuestStatus->OriginalQuestData->QuestTag);
	QuestCompletedDelegate.Broadcast(QuestStatus);
}
