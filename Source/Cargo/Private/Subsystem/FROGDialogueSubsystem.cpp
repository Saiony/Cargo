#include "Subsystem/FROGDialogueSubsystem.h"

#include "PrimaryGameLayout.h"
#include "Dialogue/ARCDialogueCallbackBase.h"
#include "Dialogue/DialogueData.h"
#include "Engine/AssetManager.h"
#include "TagDeclaration/UITypes.h"

UFROGDialogueSubsystem::UFROGDialogueSubsystem()
{
	DialogueWidgetClass = TSoftClassPtr<UDIalogueWidget>(FSoftObjectPath(TEXT("/Game/Cargo/Blueprints/UI/WBP_Dialogue.WBP_Dialogue_C")));
}

void UFROGDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UAssetManager* Manager = UAssetManager::GetIfInitialized();
	if (!Manager)
	{
		UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem: AssetManager not available"));
		return;
	}

	Manager->CallOrRegister_OnCompletedInitialScan(
		FSimpleMulticastDelegate::FDelegate::CreateLambda([this]
		{
			LoadDialogueDefinitions();
		}));
}

void UFROGDialogueSubsystem::LoadDialogueDefinitions()
{
	UAssetManager* Manager = UAssetManager::GetIfInitialized();
	if (!Manager)
	{
		return;
	}

	const FPrimaryAssetType DialogueType(TEXT("DIALOGUE_DATA"));
	Manager->GetPrimaryAssetIdList(DialogueType, CachedDialogueIds);

	if (CachedDialogueIds.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ARCDialogueSubsystem: No DIALOGUE_DATA assets found"));
		bDialoguesReady = true;
		return;
	}
	TArray<FSoftObjectPath> AssetsToLoad;
	for (const FPrimaryAssetId& Id : CachedDialogueIds)
	{
		const FSoftObjectPath Path = Manager->GetPrimaryAssetPath(Id);
		if (Path.IsValid())
		{
			AssetsToLoad.Add(Path);
		}
	}

	DialogueLoadHandle = Manager->GetStreamableManager().RequestSyncLoad(AssetsToLoad);
	OnDialoguesLoaded();
}

void UFROGDialogueSubsystem::OnDialoguesLoaded()
{
	UAssetManager* Manager = UAssetManager::GetIfInitialized();
	if (!Manager)
	{
		return;
	}

	for (const FPrimaryAssetId& Id : CachedDialogueIds)
	{
		UObject* AssetObj = Manager->GetPrimaryAssetObject(Id);
		if (auto Definition = Cast<UDialogueData>(AssetObj))
		{
			if (Definition->DialogueTag.IsValid())
			{
				DialogueRegistry.Add(Definition->DialogueTag, Definition);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ARCDialogueSubsystem: Dialogue '%s' has no DialogueTag set – skipping"),
					*Definition->GetName());
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("ARCDialogueSubsystem: Loaded %d dialogue definitions"), DialogueRegistry.Num());
	for (const auto& Pair : DialogueRegistry)
	{
		UE_LOG(LogTemp, Log, TEXT("  -> [%s] = %s"), *Pair.Key.ToString(), *Pair.Value->GetName());
	}
	bDialoguesReady = true;
	if (PendingDialogueQueue.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("ARCDialogueSubsystem: Flushing %d queued dialogue requests"), PendingDialogueQueue.Num());
	}
	PlayNextQueuedDialogue();
}

void UFROGDialogueSubsystem::PlayDialogue(UDialogueData* DialogueData, AActor* Instigator)
{
	if (!DialogueData)
	{
		UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - DialogueData is null!"));
		return;
	}
	
	if (bIsPlayingDialogue)
	{
		PendingDialogueQueue.Add({DialogueData->DialogueTag, Instigator});
		return;
	}

	CurrentInstigator = Instigator;
	CurrentDialogue = DialogueData;
	NextDialogue = DialogueData->NextDialogue;
	PushDialogueWidget(DialogueData);
	
	ACargoGameMode::Get(this)->AlreadyPlayedDialogues.Add(DialogueData->Id);
}

void UFROGDialogueSubsystem::PushDialogueWidget(UDialogueData* DialogueData)
{
	if (DialogueWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - DialogueWidgetClass is null! Path: '%s'"), *DialogueWidgetClass.ToString());
		return;
	}
	
	UPrimaryGameLayout* Layout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	if (!Layout)
	{
		UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - PrimaryGameLayout is null, cannot push dialogue widget for '%s'"), *DialogueData->DialogueTag.ToString());
		return;
	}

	bIsPlayingDialogue = true;
	UE_LOG(LogTemp, Warning, TEXT("ARCDialogueSubsystem::PlayDialogue - Pushing widget for dialogue: %s"), *DialogueData->DialogueTag.ToString());
	Layout->PushWidgetToLayerStackAsync<UDIalogueWidget>(TAG_UI_Layer_GameMenu, true, DialogueWidgetClass, [this, DialogueData](EAsyncWidgetLayerState State, UDIalogueWidget* Widget)
	{
		if (State == EAsyncWidgetLayerState::AfterPush)
		{
			if (!Widget)
			{
				UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - Widget is null after push for dialogue: %s"), *DialogueData->DialogueTag.ToString());
				bIsPlayingDialogue = false;
				return;
			}
			DialogueWidget = Widget;
			DialogueWidget->SetInstigator(CurrentInstigator.Get());
			DialogueWidget->OnDialogueFinishedDelegate.AddUObject(this, &ThisClass::OnDialogueFinished);
			ExecuteCallbacks(DialogueData->PreDialogueCallbacks, FSimpleDelegate::CreateUObject(this, &ThisClass::StartDialogueWidget));
		} 
		else if (State == EAsyncWidgetLayerState::Canceled)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARCDialogueSubsystem::PlayDialogue - Widget push canceled for dialogue: %s"), *DialogueData->DialogueTag.ToString());
			bIsPlayingDialogue = false;
		}
	});
}

void UFROGDialogueSubsystem::PlayDialogue(const FGameplayTag DialogueID, AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("ARCDialogueSubsystem::PlayDialogue called for '%s'"), *DialogueID.ToString());

	const auto FoundPtr = DialogueRegistry.Find(DialogueID);
	if (!FoundPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - Dialogue '%s' not found in registry"), *DialogueID.ToString());
		return;
	}
	
	UDialogueData* DialogueData = *FoundPtr;
	if (!DialogueData)
	{
		UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - DialogueData is null for '%s'"), *DialogueID.ToString());
		return;
	}

	PlayDialogue(DialogueData, Instigator);
}

void UFROGDialogueSubsystem::SetNextDialogue(TSoftObjectPtr<UDialogueData> DialogueData)
{
	NextDialogue = DialogueData;
}

void UFROGDialogueSubsystem::StartDialogueWidget()
{
	DialogueWidget->InitializeDialogue(CurrentDialogue);
}

void UFROGDialogueSubsystem::OnDialogueFinished(UDialogueData* DialogueData)
{
	DialogueWidget = nullptr;
	ExecuteCallbacks(DialogueData->PostDialogueCallbacks, FSimpleDelegate::CreateUObject(this, &ThisClass::FinishDialogue));
}

void UFROGDialogueSubsystem::FinishDialogue()
{
	const auto Continuation = NextDialogue;
	const auto Instigator = CurrentInstigator;
	CurrentDialogue = nullptr;
	NextDialogue.Reset();
	CurrentInstigator.Reset();
	bIsPlayingDialogue = false;
	if (!Continuation.IsNull())
	{
		PlayDialogue(Continuation.LoadSynchronous(), Instigator.Get());
	}
	PlayNextQueuedDialogue();
}

void UFROGDialogueSubsystem::PlayNextQueuedDialogue()
{
	while (PendingDialogueQueue.Num() > 0 && !bIsPlayingDialogue)
	{
		const FPendingDialogue PendingDialogue = PendingDialogueQueue[0];
		PendingDialogueQueue.RemoveAt(0);

		if (DialogueRegistry.Contains(PendingDialogue.DialogueID))
		{
			PlayDialogue(PendingDialogue.DialogueID, PendingDialogue.Instigator.Get());
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("ARCDialogueSubsystem: Queued dialogue '%s' not found in registry, skipping"), *PendingDialogue.DialogueID.ToString());
	}
}

void UFROGDialogueSubsystem::ExecuteCallbacks(const TArray<UARCDialogueCallbackBase*>& Callbacks, FSimpleDelegate Completion)
{
	ActiveCallbacks.Reset();
	CallbackIndex = 0;
	OnCallbacksCompleted = MoveTemp(Completion);
	for (auto Callback : Callbacks)
	{
		if (Callback)
		{
			ActiveCallbacks.Add(DuplicateObject<UARCDialogueCallbackBase>(Callback, this));
		}
	}
	ExecuteNextCallback();
}

void UFROGDialogueSubsystem::ExecuteNextCallback()
{
	if (ActiveCallbacks.IsValidIndex(CallbackIndex))
	{
		auto Callback = ActiveCallbacks[CallbackIndex++];
		Callback->OnCompleted.BindUObject(this, &ThisClass::ExecuteNextCallback);
		Callback->ExecuteCallback(CurrentDialogue, ACargoGameMode::Get(this), CurrentInstigator.Get());
		return;
	}
	const FSimpleDelegate Completion = MoveTemp(OnCallbacksCompleted);
	OnCallbacksCompleted.Unbind();
	ActiveCallbacks.Reset();
	Completion.ExecuteIfBound();
}
