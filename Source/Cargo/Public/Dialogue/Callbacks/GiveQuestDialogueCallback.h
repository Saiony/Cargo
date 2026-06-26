// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dialogue/ARCDialogueCallbackBase.h"
#include "Quest/QuestData.h"
#include "GiveQuestDialogueCallback.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class CARGO_API UGiveQuestDialogueCallback : public UARCDialogueCallbackBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	TObjectPtr<UQuestData> QuestData;
	
public:	
	virtual void ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator) override;
};
