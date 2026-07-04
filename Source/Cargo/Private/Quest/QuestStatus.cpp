// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestStatus.h"

void UQuestStatus::Initialize(UQuestData* QuestData, FGameplayTag StartIsland)
{
	if (!QuestData)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestStatus: Attempted to initialize with null quest data"));
		return;
	}

	OriginalQuestData = QuestData;
	QuestTag = QuestData->QuestTag;
	Title = QuestData->Title;
	DestinationTag = QuestData->DestinationTag;
	StartDeliveryDialogueTag = QuestData->StartDeliveryDialogueTag;
	EndDeliveryDialogueTag = QuestData->EndDeliveryDialogueTag;
	AlternativeEndDeliveryDialogue = QuestData->AlternativeEndDeliveryDialogue;
	
	StartIslandTag = StartIsland;
	
	for (const auto Req : QuestData->CargoRequirements)
	{
		auto CargoStatus =  FCargoStatus(Req.CargoType, 0, Req.Quantity);
		DeliveredQuantities.Add(Req.CargoType, CargoStatus);
	}
}
