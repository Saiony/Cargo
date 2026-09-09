#include "Dialogue/Callbacks/SpawnInputTextWidget_DialogueCallback.h"

#include "GameplayFramework/CargoPlayerState.h"
#include "GameFramework/PlayerController.h"

void USpawnInputTextWidget_DialogueCallback::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	Super::ExecuteCallback(DialogueDefinition, GameMode, Instigator);
	InputTextWidget.Reset();
	
	const auto UIService = GameMode->GetService<UUIService>();
	const auto Widget = UIService->ShowInputTextWidget();
	
	InputTextWidget = Widget;
	Widget->Init(TitleText, HintText, this);
}

void USpawnInputTextWidget_DialogueCallback::OnCargoInputTextConfirmed(const FString& Text)
{
	const auto Widget = InputTextWidget.Get();
	const auto PlayerController = Widget ? Widget->GetOwningPlayer() : nullptr;
	const auto PlayerState = IsValid(PlayerController) ? PlayerController->GetPlayerState<ACargoPlayerState>() : nullptr;
	
	if (!IsValid(PlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("Input text callback: Widget owner's CargoPlayerState is unavailable"));
		return;
	}

	PlayerState->AddPlayerDataTag(PlayerDataTag, Text);
	Widget->Hide();
	CompleteCallback();
}
