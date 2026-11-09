// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/Callbacks/DestroyInstigatorCallback.h"

void UDestroyInstigatorCallback::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	Super::ExecuteCallback(DialogueDefinition, GameMode, Instigator);
	
	Instigator->Destroy();
	
	CompleteCallback();
}
