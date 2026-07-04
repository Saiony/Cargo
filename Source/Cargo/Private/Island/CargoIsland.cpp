// Fill out your copyright notice in the Description page of Project Settings.


#include "Island/CargoIsland.h"

#include "Components/WidgetComponent.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Quest/QuestStatus.h"
#include "Subsystem/FROGDialogueSubsystem.h"

ACargoIsland::ACargoIsland()
{
	PrimaryActorTick.bCanEverTick = false;
	
	IslandMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IslandMesh"));
	RootComponent = IslandMeshComp;

	PortComponent = CreateDefaultSubobject<UCargoPortComponent>(TEXT("PortComponent"));
	PortComponent->SetupAttachment(RootComponent);
	
	InteractableWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableWidgetComp"));
	InteractableWidgetComp->SetupAttachment(RootComponent);
}

void ACargoIsland::BeginPlay()
{
	Super::BeginPlay();

	const auto GM = ACargoGameMode::Get(this);
	GM->QuestAcceptedDelegate.AddUObject(this, &ACargoIsland::OnQuestAccepted);
	GM->QuestCompletedDelegate.AddUObject(this, &ACargoIsland::OnQuestCompleted);
	
	Unfocus();
}

void ACargoIsland::Interact_Implementation(AActor* Interactor)
{	
	UFROGDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>();
	
	//if we have an active quest to deliver things here, play the quest dialogue instead
	if (auto ActiveQuest = ACargoGameMode::Get(this)->GetQuestStatusByDestination(LocationTag))
	{
		UE_LOG(LogTemp, Log, TEXT("Quest is active!"));		
		DialogueSubsystem->PlayDialogue(ActiveQuest->StartDeliveryDialogueTag, this);	
		PortComponent->StartQuestDelivery(ActiveQuest->QuestTag);
		
		return;
	}
	
	DialogueSubsystem->PlayDialogue(InteractionDialogueTag, this);
}

void ACargoIsland::Focus()
{
	InteractableWidgetComp->SetVisibility(true);
}

void ACargoIsland::Unfocus()
{
	InteractableWidgetComp->SetVisibility(false);	
}

void ACargoIsland::OnQuestAccepted(TObjectPtr<UQuestData> QuestData, AActor* QuestInstigator)
{
	if (QuestInstigator != this)
	{
		return;
	}

	if (!PortComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island %s: Quest accepted but PortComponent is missing!"), *LocationTag.ToString());
		return;
	}	

	UE_LOG(LogTemp, Log, TEXT("Island %s: Quest accepted! Spawning containers at PortComponent..."), *LocationTag.ToString());
	
	PortComponent->IsOpen = true;

	for (const FCargoRequirement& Req : QuestData->CargoRequirements)
	{
		for (int32 i = 0; i < Req.Quantity; ++i)
		{			
			FVector SpawnLocation = PortComponent->GetNextSpawnLocation();
			FRotator SpawnRotation = PortComponent->GetComponentRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AContainer* NewContainer = GetWorld()->SpawnActor<AContainer>(ContainerClass, SpawnLocation, SpawnRotation, SpawnParams);
			
			const auto SoftDA = GetDefault<UCargoSettings>()->ContainersMap.Find(Req.CargoType);
			auto ContainerDA = SoftDA->LoadSynchronous();
			
			NewContainer->Init(ContainerDA);
			UE_LOG(LogTemp, Log, TEXT("- Spawned %s at %s"), *NewContainer->GetName(), *SpawnLocation.ToString());
			
			//TODO: should the port keep track about this too?
		}
	}
}

void ACargoIsland::OnQuestCompleted(TObjectPtr<UQuestStatus> QuestStatus)
{
	if (QuestStatus->DestinationTag != LocationTag)
		return;
	
	UFROGDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>();

	bool ShouldPlayAlternative = true;
	for (FGameplayTag RequiredChoiceTag : QuestStatus->AlternativeEndDeliveryDialogue.RequiredChoiceTags)
	{	
		if (!ACargoGameMode::Get(this)->HasChoice(RequiredChoiceTag))
		{
			ShouldPlayAlternative = false;	
			break;
		}
	}
	
	if (ShouldPlayAlternative)
		DialogueSubsystem->PlayDialogue(QuestStatus->AlternativeEndDeliveryDialogue.AlternativeDialogue.Get()->DialogueTag, this);
	else
		DialogueSubsystem->PlayDialogue(QuestStatus->EndDeliveryDialogueTag, this);
	
	PortComponent->Clear();
}
