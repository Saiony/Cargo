// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dialogue/ARCDialogueCallbackBase.h"
#include "SpawnInputTextWidget_DialogueCallback.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API USpawnInputTextWidget_DialogueCallback : public UARCDialogueCallbackBase, public ICargoInputTextListener
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	FString TitleText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	FString HintText;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "PlayerData"), Category="Cargo")
	FGameplayTag PlayerDataTag;
	
public:
	virtual void ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator = nullptr) override;
	
	virtual void OnCargoInputTextConfirmed(const FString& Text) override;
};
