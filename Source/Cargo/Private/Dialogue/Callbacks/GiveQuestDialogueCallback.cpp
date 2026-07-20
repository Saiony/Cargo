// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/Callbacks/GiveQuestDialogueCallback.h"

void UGiveQuestDialogueCallback::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	Super::ExecuteCallback(DialogueDefinition, GameMode, Instigator);
	
	GameMode->ActivateQuest(QuestData, Instigator);	
	
	UE_LOG(LogTemp, Log, TEXT("Giving quest %s to player"), *QuestData->GetName());
}
