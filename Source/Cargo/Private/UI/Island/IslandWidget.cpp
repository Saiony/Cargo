// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Island/IslandWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"
#include "Island/CargoIsland.h"
#include "Mission/TravelMissionStatus.h"
#include "Quest/QuestStatus.h"
#include "Subsystem/FROGDialogueSubsystem.h"
#include "TagDeclaration/UITypes.h"

class UFROGDialogueSubsystem;

void UIslandWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	DialogueButton->OnClicked.AddDynamic(this, &ThisClass::OnDialogueButtonClicked);
	MissionBoardButton->OnClicked.AddDynamic(this, &ThisClass::OnMissionBoardButtonClicked);
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);	
}

void UIslandWidget::Initialize(TObjectPtr<ACargoIsland> IslandRef)
{	
	Island = IslandRef;	
	
	MissionBoardButton->SetVisibility(!ACargoGameMode::Get(this)->HasTag(TAG_InGameEvent_MissionBoardUnlocked) ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

void UIslandWidget::OnDialogueButtonClicked()
{	
	auto DialogueSubsystem = GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>();
	
	// An active quest at this destination takes priority over the island's other dialogues.
	if (auto ActiveQuest = ACargoGameMode::Get(this)->QuestService->GetQuestStatusByDestination(Island->GetLocationTag()))
	{
		//travel mission
		if (Cast<UTravelMissionStatus>(ActiveQuest->MissionStatus))
		{
			ACargoGameMode::Get(this)->QuestService->CompleteTravelQuest(ActiveQuest->QuestTag, Island);
			return;
		}
	
		//delivery mission
		if (UDialogueData* Dialogue = ActiveQuest->StartDeliveryDialogue.LoadSynchronous())
			DialogueSubsystem->PlayDialogue(Dialogue, Island);
		
		if (auto* Delivery = Cast<UDeliveryMissionStatus>(ActiveQuest->MissionStatus))
			Island->GetPort()->StartMissionDelivery(Delivery->GetId());

		return;
	}
	
	//if we have an active quest that started here, play in progress dialogue instead
	if (auto ActiveQuest = ACargoGameMode::Get(this)->QuestService->GetQuestStatusByOrigin(Island->GetLocationTag()))
	{
		UE_LOG(LogTemp, Log, TEXT("Play in progress quest dialogue"));		
		if (UDialogueData* Dialogue = ActiveQuest->InProgressDialogue.LoadSynchronous())
			DialogueSubsystem->PlayDialogue(Dialogue, Island);	
		return;
	}
	
	//if we have an available quest for this island, play start dialogue and activate it
	if (auto AvailableQuest = ACargoGameMode::Get(this)->QuestService->GetAvailableQuestByStartLocation(Island->GetLocationTag()))
	{
		UE_LOG(LogTemp, Log, TEXT("Play start quest dialogue"));		
		if (UDialogueData* Dialogue = AvailableQuest->StartDialogue.LoadSynchronous())
			DialogueSubsystem->PlayDialogue(Dialogue, Island);	
		return;
	}
	
	DialogueSubsystem->PlayDialogue(Island->GetDefaultInteractionDialogue().LoadSynchronous(), Island);
}

void UIslandWidget::OnMissionBoardButtonClicked()
{
	const auto PrimaryGameLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	const auto MissionBoardWidget = PrimaryGameLayout->PushWidgetToLayerStack<UMissionBoardWidget>(TAG_UI_Layer_Game, MissionBoardWidgetClass);
		
	const auto CargoSettings = GetDefault<UCargoSettings>();	
	const auto Missions = CargoSettings->GetMissionsDatabase()->GetMissionsForLocation(Island->GetLocationTag());
	
	MissionBoardWidget->Initialize(Missions, Island);
}

void UIslandWidget::OnCloseButtonClicked()
{
	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}
