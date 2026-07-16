// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/FROGDialogueSubsystem.h"

#include "PrimaryGameLayout.h"
#include "Dialogue/ARCDialogueCallbackBase.h"
#include "Dialogue/DialogueData.h"
#include "Engine/AssetManager.h"
#include "TagDeclaration/UITypes.h"

UFROGDialogueSubsystem::UFROGDialogueSubsystem()
{
	// Default dialogue widget – no config dependency
	DialogueWidgetClass = TSoftClassPtr<UDIalogueWidget>(FSoftObjectPath(TEXT("/Game/Cargo/Blueprints/UI/WBP_Dialogue.WBP_Dialogue_C")));
}

void UFROGDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// UGameplayMessageSubsystem::Get(this).RegisterListener<FARCGameplayEvent_DialogueStartPayload>(
	// 	TAG_GameplayEvent_StartDialogue,
	// 	this, &UARCDialogueSubsystem::OnReceivedStartDialogueMessage);

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

	// Synchronous load — dialogue definitions are small data assets and must be
	// available immediately because dialogue requests can arrive on the same frame.
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

	// Flush any dialogue requests that came in before loading finished
	if (PendingDialogueQueue.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("ARCDialogueSubsystem: Flushing %d queued dialogue requests"), PendingDialogueQueue.Num());
	}
	PlayNextQueuedDialogue();
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

	if (bIsPlayingDialogue)
	{
		PendingDialogueQueue.Add({DialogueID, Instigator});
		return;
	}

	CurrentInstigator = Instigator;
	
	if (DialogueWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - DialogueWidgetClass is null! Path: '%s'"), *DialogueWidgetClass.ToString());
		return;
	}
	
	UPrimaryGameLayout* Layout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	if (!Layout)
	{
		UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - PrimaryGameLayout is null, cannot push dialogue widget for '%s'"), *DialogueID.ToString());
		return;
	}

	bIsPlayingDialogue = true;
	UE_LOG(LogTemp, Warning, TEXT("ARCDialogueSubsystem::PlayDialogue - Pushing widget for dialogue: %s"), *DialogueID.ToString());
	Layout->PushWidgetToLayerStackAsync<UDIalogueWidget>(TAG_UI_Layer_Game, true, DialogueWidgetClass, [this, DialogueID, DialogueData](EAsyncWidgetLayerState State, UDIalogueWidget* Widget)
	{
		if (State == EAsyncWidgetLayerState::AfterPush)
		{
			if (!Widget)
			{
				UE_LOG(LogTemp, Error, TEXT("ARCDialogueSubsystem::PlayDialogue - Widget is null after push for dialogue: %s"), *DialogueID.ToString());
				bIsPlayingDialogue = false;
				return;
			}
			DialogueWidget = Widget;
			DialogueWidget->SetInstigator(CurrentInstigator.Get());
			DialogueWidget->InitializeDialogue(DialogueData);
			DialogueWidget->OnDialogueFinishedDelegate.AddUObject(this, &ThisClass::OnDialogueFinished);
		} 
		else if (State == EAsyncWidgetLayerState::Canceled)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARCDialogueSubsystem::PlayDialogue - Widget push canceled for dialogue: %s"), *DialogueID.ToString());
			bIsPlayingDialogue = false;
		}
	});
}

void UFROGDialogueSubsystem::NotifyDialogueStarted(UDialogueData* DialogueData, AActor* Instigator)
{
	if (Instigator)
	{
		CurrentInstigator = Instigator;
	}

	if (DialogueData)
	{
		HandlePreCallbacks(DialogueData);
	}
}

void UFROGDialogueSubsystem::OnDialogueFinished(UDialogueData* DialogueData)
{
	DialogueWidget = nullptr;
	bIsPlayingDialogue = false;
	
	HandlePostCallbacks(DialogueData);
	CurrentInstigator = nullptr;
	PlayNextQueuedDialogue();
}

void UFROGDialogueSubsystem::PlayNextQueuedDialogue()
{
	while (PendingDialogueQueue.Num() > 0 && !bIsPlayingDialogue)
	{
		const FPendingDialogue NextDialogue = PendingDialogueQueue[0];
		PendingDialogueQueue.RemoveAt(0);

		if (DialogueRegistry.Contains(NextDialogue.DialogueID))
		{
			PlayDialogue(NextDialogue.DialogueID, NextDialogue.Instigator.Get());
			return;
		}
		// Tag not found in registry — skip and try the next one
		UE_LOG(LogTemp, Warning, TEXT("ARCDialogueSubsystem: Queued dialogue '%s' not found in registry, skipping"), *NextDialogue.DialogueID.ToString());
	}
}

void UFROGDialogueSubsystem::HandlePreCallbacks(UDialogueData* Definition)
{
	if (Definition->PreDialogueCallbacks.Num() > 0)
	{
		for (UARCDialogueCallbackBase* Callback : Definition->PreDialogueCallbacks)
		{
			if (Callback)
			{
				Callback->ExecuteCallback(Definition, ACargoGameMode::Get(this), CurrentInstigator.Get());
			}
		}
	}	
}

void UFROGDialogueSubsystem::HandlePostCallbacks(UDialogueData* Definition)
{
	if (Definition->PostDialogueCallbacks.Num() > 0)
	{
		for (UARCDialogueCallbackBase* Callback : Definition->PostDialogueCallbacks)
		{
			if (Callback)
			{
				Callback->ExecuteCallback(Definition, ACargoGameMode::Get(this), CurrentInstigator.Get());
			}
		}
	}
}
