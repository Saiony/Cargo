// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dialogue/ARCDialogueCallbackBase.h"
#include "SpawnCargoCallback.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API USpawnCargoCallback : public UARCDialogueCallbackBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCargoRequirement CargoToSpawn;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo", meta=(ClampMin="1"))
	FIntPoint GridSizeXY = FIntPoint(2, 2);

public:
	virtual void ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator = nullptr) override;
};
