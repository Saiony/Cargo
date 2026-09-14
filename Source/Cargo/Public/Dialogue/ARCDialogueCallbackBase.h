#pragma once

#include "CoreMinimal.h"
#include "CargoGameMode.h"
#include "DialogueData.h"
#include "UObject/Object.h"
#include "ARCDialogueCallbackBase.generated.h"

UCLASS(EditInlineNew, Abstract)
class CARGO_API UARCDialogueCallbackBase : public UObject
{
	GENERATED_BODY()
	
public:
	FSimpleDelegate OnCompleted;

	virtual void ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator = nullptr);

protected:
	void CompleteCallback();
};
