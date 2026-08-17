// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestStatus.h"

void UQuestStatus::Initialize(UQuestData* QuestData)
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
	StartDeliveryDialogue = QuestData->StartDeliveryDialogue;
	EndDeliveryDialogue = QuestData->EndDeliveryDialogue;
	InProgressDialogue = QuestData->InProgressDialogue;
	AlternativeEndDeliveryDialogue = QuestData->AlternativeEndDeliveryDialogue;
	Reward = QuestData->Reward;
	
	NextQuest = QuestData->NextQuest;
	StartIslandTag = QuestData->StartLocationTag;
	
	for (const auto Req : QuestData->CargoRequirements)
	{
		auto CargoStatus =  FCargoStatus(Req.CargoType, 0, Req.Quantity);
		DeliveredQuantities.Add(Req.CargoType, CargoStatus);
	}
}
