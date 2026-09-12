#include "Dialogue/Callbacks/AddAvailableQuestDialogueCallback.h"

void UAddAvailableQuestDialogueCallback::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	Super::ExecuteCallback(DialogueDefinition, GameMode, Instigator);
	
	GameMode->QuestService->AddAvailableQuest(QuestData);	
	
	UE_LOG(LogTemp, Log, TEXT("Giving quest %s to player"), *QuestData->GetName());
	CompleteCallback();
}
