// Fill out your copyright notice in the Description page of Project Settings.


#include "Island/CargoIsland.h"

#include "PrimaryGameLayout.h"
#include "Components/WidgetComponent.h"
#include "Quest/QuestStatus.h"
#include "Subsystem/FROGDialogueSubsystem.h"
#include "TagDeclaration/UITypes.h"
#include "UI/Island/IslandWidget.h"

ACargoIsland::ACargoIsland()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	IslandMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IslandMesh"));
	IslandMeshComp->SetupAttachment(RootComponent);

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
	
	const auto MissionsService = GM->MissionsService;
	MissionsService->MissionAcceptedDelegate.AddUObject(this, &ThisClass::OnMissionAccepted);
	MissionsService->MissionCompletedDelegate.AddUObject(this, &ThisClass::OnMissionCompleted);
	
	Unfocus();
}

void ACargoIsland::Interact_Implementation(AActor* Interactor)
{	
	UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());

	const auto PrimaryGameLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	const auto IslandWidget = PrimaryGameLayout->PushWidgetToLayerStack<UIslandWidget>(TAG_UI_Layer_Game, IslandWidgetClass.LoadSynchronous());
	
	IslandWidget->Initialize(this);
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
		return;

	if (!PortComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island %s: Quest accepted but PortComponent is missing!"), *LocationTag.ToString());
		return;
	}	

	UE_LOG(LogTemp, Log, TEXT("Island %s: Quest accepted! Spawning containers at PortComponent..."), *LocationTag.ToString());
	
	PortComponent->IsOpen = true;
	
	if (QuestData->IsDeliveryOnly)
		return;
	
	PortComponent->SpawnCargo(QuestData->CargoRequirements);
}

void ACargoIsland::OnQuestCompleted(TObjectPtr<UQuestStatus> QuestStatus)
{
	if (QuestStatus->DestinationTag != LocationTag)
		return;
	
	UFROGDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>();

	bool ShouldPlayAlternative = false;
	for (FGameplayTag RequiredChoiceTag : QuestStatus->AlternativeEndDeliveryDialogue.RequiredChoiceTags)
	{	
		if (ACargoGameMode::Get(this)->HasChoice(RequiredChoiceTag))
		{
			ShouldPlayAlternative = true;	
			break;
		}
	}
	
	if (ShouldPlayAlternative)
		DialogueSubsystem->PlayDialogue(QuestStatus->AlternativeEndDeliveryDialogue.AlternativeDialogue.LoadSynchronous(), this);
	else
		DialogueSubsystem->PlayDialogue(QuestStatus->EndDeliveryDialogue.LoadSynchronous(), this);
	
	PortComponent->Clear();
}

void ACargoIsland::OnMissionAccepted(TObjectPtr<UMissionStatus> MissionStatus, FGameplayTag InstigatorIslandTag)
{
	if (InstigatorIslandTag != LocationTag)
		return;
	
	if (!PortComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island %s: Mission accepted but PortComponent is missing!"), *LocationTag.ToString());
		return;
	}	

	UE_LOG(LogTemp, Log, TEXT("Island %s: Mission accepted! Spawning containers at PortComponent..."), *LocationTag.ToString());
	
	PortComponent->IsOpen = true;
	
	PortComponent->SpawnCargo(MissionStatus->GetOriginalMissionData()->CargoRequirements);
}

void ACargoIsland::OnMissionCompleted(TObjectPtr<UMissionStatus> MissionStatus)
{
	if (MissionStatus->GetDestinationTag() != LocationTag)
		return;
	
	if (!PortComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island %s: Mission accepted but PortComponent is missing!"), *LocationTag.ToString());
		return;
	}	

	UE_LOG(LogTemp, Log, TEXT("Island %s: Mission completed!"), *LocationTag.ToString());
	
	PortComponent->Clear();
}
