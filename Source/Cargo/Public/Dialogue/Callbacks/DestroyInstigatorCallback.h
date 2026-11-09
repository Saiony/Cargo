// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dialogue/ARCDialogueCallbackBase.h"
#include "DestroyInstigatorCallback.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UDestroyInstigatorCallback : public UARCDialogueCallbackBase
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator = nullptr) override;
};
