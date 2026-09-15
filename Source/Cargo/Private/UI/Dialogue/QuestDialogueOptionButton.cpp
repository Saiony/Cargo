#include "UI/Dialogue/QuestDialogueOptionButton.h"

#include "CargoGameMode.h"
#include "CommonTextBlock.h"
#include "Island/CargoIsland.h"
#include "Mission/TravelMissionStatus.h"
#include "Quest/QuestStatus.h"
#include "Subsystem/FROGDialogueSubsystem.h"

void UQuestDialogueOptionButton::Init(UQuestData* InQuestData, EQuestDialogueOptionType InType, ACargoIsland* InIsland, IQuestDialogueOptionListener* InListener)
{
	QuestData = InQuestData;
	Type = InType;
	Island = InIsland;
	Listener = InListener;
	Text->SetText(QuestData->Title);
}

void UQuestDialogueOptionButton::NativeOnClicked()
{
	Super::NativeOnClicked();
	switch (Type)
	{
	case EQuestDialogueOptionType::StartQuest:
		// Quest acceptance is handled by the dialogue's callback.
		PlayDialogue(QuestData->StartDialogue);
		break;
	case EQuestDialogueOptionType::QuestInProgress:
		PlayDialogue(QuestData->InProgressDialogue);
		break;
	case EQuestDialogueOptionType::EndQuest:
		EndQuest();
		break;
	}
	Listener->OnQuestDialogueOptionClicked(this);
}

void UQuestDialogueOptionButton::PlayDialogue(TSoftObjectPtr<UDialogueData> Dialogue)
{
	const auto Data = Dialogue.LoadSynchronous();
	check(Data);
	
	GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>()->PlayDialogue(Data, Island);
}

void UQuestDialogueOptionButton::EndQuest()
{
	const auto QuestService = ACargoGameMode::Get(this)->QuestService;
	const auto Quests = QuestService->GetQuestStatus(QuestData->QuestTag);
	if (Quests.IsEmpty())
		return;
	const auto Quest = Quests[0];
	check(Quest);

	const auto Travel = Cast<UTravelMissionStatus>(Quest->MissionStatus);
	if (Travel)
	{
		QuestService->CompleteTravelQuest(QuestData->QuestTag, Island);
		return;
	}

	const auto Delivery = CastChecked<UDeliveryMissionStatus>(Quest->MissionStatus);
	if (Delivery)
	{
		PlayDialogue(Quest->StartDeliveryDialogue);
		Island->GetPort()->StartMissionDelivery(Delivery->GetId());
	}
}
