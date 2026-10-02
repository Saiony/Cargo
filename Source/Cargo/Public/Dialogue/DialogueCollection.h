#pragma once

#include "CoreMinimal.h"
#include "Dialogue/DialogueData.h"
#include "DialogueCollection.generated.h"

USTRUCT(BlueprintType)
struct CARGO_API FDialogueCollection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue")
	TArray<TObjectPtr<UDialogueData>> Dialogues;
};
