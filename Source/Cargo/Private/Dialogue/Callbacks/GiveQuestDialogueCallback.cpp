#include "Dialogue/Callbacks/GiveQuestDialogueCallback.h"

void UGiveQuestDialogueCallback::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	Super::ExecuteCallback(DialogueDefinition, GameMode, Instigator);
	
	if (!IsValid(GameMode) || !IsValid(GameMode->QuestService))
	{
		UE_LOG(LogTemp, Error, TEXT("GiveQuestDialogueCallback: Cannot activate quest '%s' from dialogue '%s': GameMode or QuestService is invalid."),
			*GetPathNameSafe(QuestData), *GetPathNameSafe(DialogueDefinition));
		CompleteCallback();
		return;
	}

	if (GameMode->QuestService->ActivateQuest(QuestData, Instigator))
	{
		UE_LOG(LogTemp, Log, TEXT("GiveQuestDialogueCallback: Activated quest '%s' from dialogue '%s'."),
			*GetPathNameSafe(QuestData), *GetPathNameSafe(DialogueDefinition));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GiveQuestDialogueCallback: Failed to activate quest '%s' from dialogue '%s', callback '%s', instigator '%s'. See the preceding QuestService::ActivateQuest error for the reason."),
			*GetPathNameSafe(QuestData), *GetPathNameSafe(DialogueDefinition), *GetPathName(), *GetPathNameSafe(Instigator));
	}
	CompleteCallback();
}
