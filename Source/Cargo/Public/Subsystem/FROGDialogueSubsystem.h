#pragma once

#include "CoreMinimal.h"
#include "Dialogue/DialogueData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/DIalogueWidget.h"
#include "FROGDialogueSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FPendingDialogue
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag DialogueID;

	UPROPERTY()
	TWeakObjectPtr<AActor> Instigator;
};

UCLASS()
class CARGO_API UFROGDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFROGDialogueSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void PlayDialogue(UDialogueData* DialogueData, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category="Arcade")
	void PlayDialogue(FGameplayTag DialogueID, AActor* Instigator = nullptr);

	void SetNextDialogue(TSoftObjectPtr<UDialogueData> DialogueData);

protected:
	void PlayNextQueuedDialogue();
	void OnDialogueFinished(UDialogueData* DialogueData);
	void StartDialogueWidget();
	void FinishDialogue();
	void ExecuteCallbacks(const TArray<UARCDialogueCallbackBase*>& Callbacks, FSimpleDelegate Completion);
	void ExecuteNextCallback();
	void PushDialogueWidget(UDialogueData* DialogueData);

private:

	void LoadDialogueDefinitions();


	void OnDialoguesLoaded();


	UPROPERTY(Transient)
	TArray<FPrimaryAssetId> CachedDialogueIds;


	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UDialogueData>> DialogueRegistry;


	UPROPERTY()
	TSoftClassPtr<UDIalogueWidget> DialogueWidgetClass;

	UPROPERTY()
	TObjectPtr<UDIalogueWidget> DialogueWidget;

	UPROPERTY(Transient)
	TObjectPtr<UDialogueData> CurrentDialogue;

	UPROPERTY(Transient)
	TSoftObjectPtr<UDialogueData> NextDialogue;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UARCDialogueCallbackBase>> ActiveCallbacks;

	int32 CallbackIndex = 0;
	FSimpleDelegate OnCallbacksCompleted;

	TSharedPtr<struct FStreamableHandle> DialogueLoadHandle;


	bool bDialoguesReady = false;


	bool bIsPlayingDialogue = false;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> CurrentInstigator;


	UPROPERTY()
	TArray<FPendingDialogue> PendingDialogueQueue;
};
