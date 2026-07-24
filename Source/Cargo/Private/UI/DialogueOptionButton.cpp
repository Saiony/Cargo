// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DialogueOptionButton.h"

#include "CommonTextBlock.h"

void UDialogueOptionButton::Init(const FCargoDialogueChoice& Choice)
{
	Text->SetText(Choice.Text);
}
