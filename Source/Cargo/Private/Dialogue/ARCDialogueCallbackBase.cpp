// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/ARCDialogueCallbackBase.h"

void UARCDialogueCallbackBase::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Executing dialogue callback: %s"), *GetName());
}