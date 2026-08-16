// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Island/IslandWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"
#include "Island/CargoIsland.h"
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
	
	//if we have an active quest to deliver things here, play the quest dialogue instead
	if (auto ActiveQuest = ACargoGameMode::Get(this)->GetQuestStatusByDestination(Island->GetLocationTag()))
	{
		UE_LOG(LogTemp, Log, TEXT("Play start delivery quest dialogue"));		
		DialogueSubsystem->PlayDialogue(ActiveQuest->StartDeliveryDialogue.LoadSynchronous()->DialogueTag, Island);	
		Island->GetPort()->StartQuestDelivery(ActiveQuest->QuestTag);
		
		if (ActiveQuest->DeliveredQuantities.Num() == 0)
		{
			ACargoGameMode::Get(this)->CheckIfQuestEnded(ActiveQuest);
		}
		
		return;
	}
	
	//if we have an active quest that started here, play in progress dialogue instead
	if (auto ActiveQuest = ACargoGameMode::Get(this)->GetQuestStatusByOrigin(Island->GetLocationTag()))
	{
		UE_LOG(LogTemp, Log, TEXT("Play in progress quest dialogue"));		
		DialogueSubsystem->PlayDialogue(ActiveQuest->InProgressDialogue.LoadSynchronous()->DialogueTag, Island);	
		return;
	}
	
	//if we have an available quest for this island, play start dialogue and activate it
	if (auto AvailableQuest = ACargoGameMode::Get(this)->GetAvailableQuestByStartLocation(Island->GetLocationTag()))
	{
		UE_LOG(LogTemp, Log, TEXT("Play start quest dialogue"));		
		DialogueSubsystem->PlayDialogue(AvailableQuest->StartDialogue.LoadSynchronous()->DialogueTag, Island);	
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
