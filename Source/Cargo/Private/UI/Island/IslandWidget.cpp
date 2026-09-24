// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Island/IslandWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"
#include "Components/VerticalBoxSlot.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Island/CargoIsland.h"
#include "Island/IslandStoreOptionData.h"
#include "Quest/QuestStatus.h"
#include "Services/UIService.h"
#include "TagDeclaration/UITypes.h"
#include "UI/Generic/GenericButton.h"
#include "UI/Shop/StoreWidget.h"

void UIslandWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MissionBoardButton->OnClicked.AddDynamic(this, &ThisClass::OnMissionBoardButtonClicked);
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
}
void UIslandWidget::Initialize(TObjectPtr<ACargoIsland> IslandRef)
{
	Island = IslandRef;

	MissionBoardButton->SetVisibility(!ACargoGameMode::Get(this)->HasInGameEventTag(TAG_InGameEvent_MissionBoardUnlocked) ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

	OptionsContainer->ClearChildren();
	DrawDialogueButtons();
	DrawIslandOptions();
	
	DefaultDialogueButton->Init(FText::FromString("Conversar"), IslandRef->GetIslandData()->DefaultInteractionDialogue.LoadSynchronous(), IslandRef, this);
}

void UIslandWidget::DrawIslandOptions()
{
	for (const auto& Option : Island->GetIslandData()->Options)
	{
		const auto StoreOption = Cast<UIslandStoreOptionData>(Option);
		if (!StoreOption)
			continue;

		const auto GenericButtonClass = GetDefault<UCargoSettings>()->GenericButtonClass.LoadSynchronous();
		const auto Button = CreateWidget<UGenericButton>(this, GenericButtonClass);
		Button->SetText(StoreOption->GetButtonText());
		Button->OnClicked().AddWeakLambda(this, [this, StoreOption]()
		{
			const auto UIService = ACargoGameMode::Get(this)->GetService<UUIService>();
			const auto StoreWidgetClass = GetDefault<UCargoSettings>()->StoreWidgetClass.LoadSynchronous();
			const auto Widget = UIService->ShowWidget<UStoreWidget>(StoreWidgetClass);
			Widget->Init(StoreOption->StoreCatalog, Island);
		});

		const auto ChildrenSlot = OptionsContainer->AddChildToVerticalBox(Button);
		ChildrenSlot->SetPadding(FMargin(0.f, 0.f, 0.f, DialogueOptionsContainerPadding));
	}
}

void UIslandWidget::DrawDialogueButtons()
{
	const auto QuestService = ACargoGameMode::Get(this)->QuestService;
	const auto LocationTag = Island->GetLocationTag();

	// Quests that start here
	const auto AvailableQuests = QuestService->GetAvailableQuestsByStartLocation(LocationTag);
	for (const auto& AvailableQuest : AvailableQuests)
	{			
		CreateQuestDialogueOptionButton(AvailableQuest, EQuestDialogueOptionType::StartQuest);
	}	
	
	// Quests that end here
	const auto ActiveQuests = QuestService->GetQuestStatusByDestination(LocationTag);
	for (const auto& ActiveQuest : ActiveQuests)
	{
		CreateQuestDialogueOptionButton(ActiveQuest->OriginalQuestData, EQuestDialogueOptionType::EndQuest);
	}

	// Quests that start here, but are in progress
	const auto OriginQuests = QuestService->GetQuestsStatusByOrigin(LocationTag);
	for (const auto& OriginQuest : OriginQuests)
	{
		if (!ActiveQuests.Contains(OriginQuest))
			CreateQuestDialogueOptionButton(OriginQuest->OriginalQuestData, EQuestDialogueOptionType::QuestInProgress);
	}
	
	//Simple dialogues based on in-game events
	for (const auto DialogueByInGameEvent : Island->GetIslandData()->DialoguesByRequiredEvent)
	{
		if (!ACargoGameMode::Get(this)->HasInGameEventTag(DialogueByInGameEvent.Key))
			continue;
		
		for (const auto Dialogue : DialogueByInGameEvent.Value.Dialogues)
		{
			if (ACargoGameMode::Get(this)->AlreadyPlayedDialogues.Contains(Dialogue->Id))
				continue;
			
			CreateDialogueOptionButton(Dialogue->Title, Dialogue);			
		}
	}
}

void UIslandWidget::CreateQuestDialogueOptionButton(TObjectPtr<UQuestData> Quest, EQuestDialogueOptionType Type)
{
	const auto OptionButton = CreateWidget<UQuestDialogueOptionButton>(this, QuestDialogueOptionButtonClass);
	OptionButton->Init(Quest, Type, Island, this);
	
	const auto ChildrenSlot = OptionsContainer->AddChildToVerticalBox(OptionButton);
	ChildrenSlot->SetPadding(FMargin(0.f, 0.f, 0.f, DialogueOptionsContainerPadding)); 
}

void UIslandWidget::CreateDialogueOptionButton(const FText& Title, UDialogueData* Dialogue)
{
	const auto OptionButton = CreateWidget<UDialogueOptionButton>(this, DialogueOptionButtonClass);
	OptionButton->Init(Title, Dialogue, Island, this);
	
	const auto ChildrenSlot = OptionsContainer->AddChildToVerticalBox(OptionButton);
	ChildrenSlot->SetPadding(FMargin(0.f, 0.f, 0.f, DialogueOptionsContainerPadding)); 
}

void UIslandWidget::OnQuestDialogueOptionClicked(UQuestDialogueOptionButton* Button)
{
}

void UIslandWidget::OnDialogueOptionClicked(UDialogueOptionButton* Button, const int8 Id)
{
	if (Button != DefaultDialogueButton)
		Button->SetIsEnabled(false);
}

void UIslandWidget::OnMissionBoardButtonClicked()
{
	const auto PrimaryGameLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	const auto MissionBoardWidget = PrimaryGameLayout->PushWidgetToLayerStack<UMissionBoardWidget>(TAG_UI_Layer_GameMenu, MissionBoardWidgetClass);

	const auto CargoSettings = GetDefault<UCargoSettings>();
	const auto Missions = CargoSettings->GetMissionsDatabase()->GetMissionsForLocation(Island->GetLocationTag());

	MissionBoardWidget->Initialize(Missions, Island);
}

void UIslandWidget::OnCloseButtonClicked()
{
	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}
