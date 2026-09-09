// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/Callbacks/SpawnInputTextWidget_DialogueCallback.h"

#include "GameplayFramework/CargoPlayerState.h"

void USpawnInputTextWidget_DialogueCallback::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	Super::ExecuteCallback(DialogueDefinition, GameMode, Instigator);
	
	const auto UIService = GameMode->GetService<UUIService>();	
	const auto Widget = UIService->ShowInputTextWidget();
	
	Widget->Init(TitleText, HintText, this);
}

void USpawnInputTextWidget_DialogueCallback::OnCargoInputTextConfirmed(const FString& Text)
{
	const auto PlayerState =  GetWorld()->GetFirstPlayerController()->GetPlayerState<ACargoPlayerState>();	
	PlayerState->AddPlayerDataTag(PlayerDataTag, Text);	
}
