// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/Callbacks/AddAvailableQuestDialogueCallback.h"

void UAddAvailableQuestDialogueCallback::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	Super::ExecuteCallback(DialogueDefinition, GameMode, Instigator);
	
	GameMode->AddAvailableQuest(QuestData);	
	
	UE_LOG(LogTemp, Log, TEXT("Giving quest %s to player"), *QuestData->GetName());
}
