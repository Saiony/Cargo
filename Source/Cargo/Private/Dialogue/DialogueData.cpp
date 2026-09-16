// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/DialogueData.h"

UDialogueData::UDialogueData()
{
	Id = FGuid::NewGuid();
}

void UDialogueData::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	
	Id = FGuid::NewGuid();
}
