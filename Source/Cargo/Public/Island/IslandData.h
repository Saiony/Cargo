// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Dialogue/DialogueCollection.h"
#include "Island/IslandOptionData.h"
#include "IslandData.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UIslandData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Instanced, Category="Interaction")
	TArray<TObjectPtr<UIslandOptionData>> Options;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Location"))
	FGameplayTag LocationTag;
	
	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue")
	TSoftObjectPtr<UDialogueData> DefaultInteractionDialogue;
	
	// Dialogues available after the event identified by the key has occurred.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue", meta=(Categories="InGameEvent"))
	TMap<FGameplayTag, FDialogueCollection> DialoguesByRequiredEvent;
};
