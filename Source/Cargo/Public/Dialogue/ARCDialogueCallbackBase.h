// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CargoGameMode.h"
#include "DialogueData.h"
#include "UObject/Object.h"
#include "ARCDialogueCallbackBase.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UARCDialogueCallbackBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode);
	
private:
	/** Cached world context set before ExecuteTask is invoked. */
	UPROPERTY(Transient)
	TObjectPtr<ACargoGameMode> CachedGameMode;
};
