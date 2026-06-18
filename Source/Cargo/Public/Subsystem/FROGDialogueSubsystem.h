// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dialogue/DialogueData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/DIalogueWidget.h"
#include "FROGDialogueSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UFROGDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFROGDialogueSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category="Arcade")
	void PlayDialogue(FGameplayTag DialogueID);
	
protected:
	//void OnReceivedStartDialogueMessage(FGameplayTag GameplayTag, const FARCGameplayEvent_DialogueStartPayload& Payload);
	void PlayNextQueuedDialogue();
	void OnDialogueFinished(UDialogueData* DialogueData);
	void HandlePreCallbacks(UDialogueData* Definition);
	void HandlePostCallbacks(UDialogueData* Definition);
	
private:
	/** Discovers all DIALOGUE_DATA primary assets via Asset Manager and begins async load. */
	void LoadDialogueDefinitions();

	/** Called when all dialogue definitions have finished loading. Builds the tag→definition map. */
	void OnDialoguesLoaded();

	/** Primary asset IDs discovered from the Asset Manager scan. */
	UPROPERTY(Transient)
	TArray<FPrimaryAssetId> CachedDialogueIds;

	/** Runtime registry built after assets are loaded. Maps DialogueTag → loaded definition. */
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UDialogueData>> DialogueRegistry;

	/** Soft class path to the dialogue widget blueprint – set in constructor. */
	UPROPERTY()
	TSoftClassPtr<UDIalogueWidget> DialogueWidgetClass;

	UPROPERTY()
	TObjectPtr<UDIalogueWidget> DialogueWidget;

	TSharedPtr<struct FStreamableHandle> DialogueLoadHandle;

	/** True once all dialogue definitions have been loaded and the registry is populated. */
	bool bDialoguesReady = false;

	/** True while a dialogue widget is on screen. Prevents overlapping pushes. */
	bool bIsPlayingDialogue = false;

	/** Queued dialogue IDs waiting to be played after the current one finishes. */
	TArray<FGameplayTag> PendingDialogueQueue;
};
