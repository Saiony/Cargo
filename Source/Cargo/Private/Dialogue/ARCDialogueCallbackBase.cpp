#include "Dialogue/ARCDialogueCallbackBase.h"

void UARCDialogueCallbackBase::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Executing dialogue callback: %s"), *GetName());
}

void UARCDialogueCallbackBase::CompleteCallback()
{
	const FSimpleDelegate Completion = MoveTemp(OnCompleted);
	OnCompleted.Unbind();
	Completion.ExecuteIfBound();
}
