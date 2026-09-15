#include "Services/QuestService.h"

#include "CargoGameMode.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "Engine/GameInstance.h"
#include "Island/CargoIsland.h"
#include "Mission/MissionReward.h"
#include "Mission/TravelMissionStatus.h"
#include "Subsystem/CargoUIManagerSubsystem.h"
#include "Subsystem/FROGDialogueSubsystem.h"

UQuestService::UQuestService()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestService::PostLogin(APlayerController* NewPlayer)
{
	if (auto* PlayerState = NewPlayer->GetPlayerState<ACargoPlayerState>())
		PlayerState->OnShipCollisionEvent.AddUObject(this, &ThisClass::OnShipCollision);
}

void UQuestService::Logout(AController* Exiting)
{
	if (auto* PlayerState = Exiting->GetPlayerState<ACargoPlayerState>())
		PlayerState->OnShipCollisionEvent.RemoveAll(this);
}

void UQuestService::OnShipCollision(AActor* OtherActor, ShipCollisionType CollisionType)
{
	const auto AddCollision = [CollisionType](UDeliveryMissionStatus* Mission)
	{
		if (!Mission)
			return;
		if (CollisionType == ShipCollisionType::Light)
			Mission->AddCollision_Light();
		else if (CollisionType == ShipCollisionType::Heavy)
			Mission->AddCollision_Hard();
	};
	for (const auto& Entry : ActiveMissions)
		AddCollision(Entry.Value);
	for (const auto& Entry : ActiveQuests)
		AddCollision(Cast<UDeliveryMissionStatus>(Entry.Value->MissionStatus));
}

void UQuestService::AcceptMission(TObjectPtr<UDeliveryMissionData> MissionData, FGameplayTag StartIslandTag)
{
	if (!IsValid(MissionData) || ActiveMissions.Contains(MissionData->GetId()))
		return;

	auto* Mission = NewObject<UDeliveryMissionStatus>(this);
	Mission->Initialize(MissionData, StartIslandTag);
	ActiveMissions.Add(Mission->GetId(), Mission);
	ActiveMissionsUpdatedDelegate.Broadcast(ActiveMissions);
	MissionAcceptedDelegate.Broadcast(Mission, StartIslandTag);
}

UQuestStatus* UQuestService::FindQuestForMission(FGuid MissionId) const
{
	for (const auto& Entry : ActiveQuests)
	{
		const auto* Delivery = Cast<UDeliveryMissionStatus>(Entry.Value->MissionStatus);
		if (Delivery && Delivery->GetId() == MissionId)
			return Entry.Value;
	}
	return nullptr;
}

UDeliveryMissionStatus* UQuestService::FindMission(FGuid MissionId) const
{
	if (const auto* Mission = ActiveMissions.Find(MissionId))
		return *Mission;
	if (const auto* Quest = FindQuestForMission(MissionId))
		return Cast<UDeliveryMissionStatus>(Quest->MissionStatus);
	return nullptr;
}

void UQuestService::RegisterCargoDelivery(FGuid MissionId, FGameplayTag CargoType)
{
	UpdateCargoDelivery(MissionId, CargoType, 1);
}

void UQuestService::RemoveCargoDelivery(FGuid MissionId, FGameplayTag CargoType)
{
	UpdateCargoDelivery(MissionId, CargoType, -1);
}

void UQuestService::UpdateCargoDelivery(FGuid MissionId, FGameplayTag CargoType, int32 Quantity)
{
	auto* Mission = FindMission(MissionId);
	if (!Mission || !Mission->GetDeliveredQuantities().Contains(CargoType))
		return;

	Mission->AddCargo(CargoType, Quantity);
	if (auto* Quest = FindQuestForMission(MissionId))
	{
		const auto Quantities = Mission->GetDeliveredQuantities();
		if (const auto* Cargo = Quantities.Find(CargoType))
			OnQuestProgressUpdatedDelegate.Broadcast(Quest, CargoType, Cargo->DeliveredQuantity);
	}
	else
		MissionProgressUpdatedDelegate.Broadcast(Mission, CargoType);
}

void UQuestService::CompleteMission(FGuid MissionId, AActor* InstigatorIsland)
{
	auto Mission = FindMission(MissionId);
	const auto Island = Cast<ACargoIsland>(InstigatorIsland);
	
	if (!Mission || !Island || Island->GetLocationTag() != Mission->GetDestinationTag())
	{
		UE_LOG(LogTemp, Error, TEXT("QuestService: Cannot complete mission %s. Mission is not active or not in the same island."), *MissionId.ToString());
		return;
	}

	auto GameMode = ACargoGameMode::Get(this);
	auto Quest = FindQuestForMission(MissionId);
	TSoftObjectPtr<UDialogueData> EndDialogue;
	
	if (Quest)
	{
		EndDialogue = Quest->EndDeliveryDialogue;
		for (const FGameplayTag Tag : Quest->AlternativeEndDeliveryDialogue.RequiredChoiceTags)
		{
			if (GameMode->HasTag(Tag))
			{
				EndDialogue = Quest->AlternativeEndDeliveryDialogue.AlternativeDialogue;
				break;
			}
		}
		ActiveQuests.Remove(Quest->QuestTag);
		if (Quest->Reward.RewardTag.IsValid())
			GameMode->AddTag(Quest->Reward.RewardTag);
		GameMode->EconomyService->AddMoney(Quest->Reward.Money);
		AddAvailableQuest(Quest->NextQuest.LoadSynchronous());
	}
	else
		ActiveMissions.Remove(MissionId);

	
	const FMissionReward Result = Mission->CompleteMission();
	GameMode->EconomyService->AddMoney(Result.FinalReward.Money);
	MissionCompletedDelegate.Broadcast(Mission);
	
	if (Quest)
		QuestCompletedDelegate.Broadcast(Quest);

	const TWeakObjectPtr<AActor> DialogueInstigator = InstigatorIsland;
	auto* UIManager = GetWorld()->GetGameInstance()->GetSubsystem<UCargoUIManagerSubsystem>();
	check(UIManager);
	
	UIManager->ShowMissionResult(Mission, FSimpleDelegate::CreateWeakLambda(this, [this, EndDialogue, DialogueInstigator]()
	{
		if (EndDialogue.IsNull())
			return;
		
		check(DialogueInstigator.IsValid());
		
		UDialogueData* Dialogue = EndDialogue.LoadSynchronous();
		check(Dialogue);
		
		const auto DialogueSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>();
		check(DialogueSubsystem);
		
		DialogueSubsystem->PlayDialogue(Dialogue, DialogueInstigator.Get());
	}));
}

TArray<TObjectPtr<UDeliveryMissionStatus>> UQuestService::GetActiveMissionsForDestination(FGameplayTag DestinationTag)
{
	TArray<TObjectPtr<UDeliveryMissionStatus>> Missions;
	for (const auto& Entry : ActiveMissions)
	{
		if (Entry.Value->GetDestinationTag() == DestinationTag)
			Missions.Add(Entry.Value);
	}
	return Missions;
}

void UQuestService::ActivateQuest(UQuestData* QuestData, AActor* QuestInstigator)
{
	const auto* Island = Cast<ACargoIsland>(QuestInstigator);
	if (!IsValid(QuestData) || !Island || Island->GetLocationTag() != QuestData->StartLocationTag)
		return;
	if (!AvailableQuests.Contains(QuestData) || ActiveQuests.Contains(QuestData->QuestTag))
		return;

	auto* Quest = NewObject<UQuestStatus>(this);
	Quest->Initialize(QuestData);
	if (!Quest->MissionStatus)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestService: Cannot accept %s. Assign a supported MissionData asset to the quest."), *QuestData->GetPathName());
		return;
	}

	ActiveQuests.Add(Quest->QuestTag, Quest);
	AvailableQuests.Remove(QuestData);
	ActiveQuestsDelegate.Broadcast(ActiveQuests);
	QuestAcceptedDelegate.Broadcast(QuestData, QuestInstigator);
	if (auto* Delivery = Cast<UDeliveryMissionStatus>(Quest->MissionStatus))
		MissionAcceptedDelegate.Broadcast(Delivery, Island->GetLocationTag());
}

void UQuestService::CompleteTravelQuest(FGameplayTag QuestTag, AActor* InstigatorIsland)
{
	const auto Quests = GetQuestStatus(QuestTag);
	const auto Quest = Quests.IsEmpty() ? nullptr : Quests[0];
	const auto* Island = Cast<ACargoIsland>(InstigatorIsland);
	const auto* Travel = Quest ? Cast<UTravelMissionStatus>(Quest->MissionStatus) : nullptr;
	if (!Travel || !Island || !Travel->GetDestinationTag().IsValid()
		|| Island->GetLocationTag() != Travel->GetDestinationTag())
		return;

	auto* GameMode = ACargoGameMode::Get(this);
	auto EndDialogue = Quest->EndDeliveryDialogue;
	for (const FGameplayTag Tag : Quest->AlternativeEndDeliveryDialogue.RequiredChoiceTags)
	{
		if (GameMode->HasTag(Tag))
		{
			EndDialogue = Quest->AlternativeEndDeliveryDialogue.AlternativeDialogue;
			break;
		}
	}

	// Remove before rewards and callbacks so repeated interactions cannot complete it twice.
	ActiveQuests.Remove(QuestTag);
	if (Quest->Reward.RewardTag.IsValid())
		GameMode->AddTag(Quest->Reward.RewardTag);
	GameMode->EconomyService->AddMoney(Quest->Reward.Money);
	AddAvailableQuest(Quest->NextQuest.LoadSynchronous());
	QuestCompletedDelegate.Broadcast(Quest);

	if (UDialogueData* Dialogue = EndDialogue.LoadSynchronous())
	{
		auto* DialogueSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>();
		check(DialogueSubsystem);
		DialogueSubsystem->PlayDialogue(Dialogue, InstigatorIsland);
	}
}

void UQuestService::AddAvailableQuest(TObjectPtr<UQuestData> Quest)
{
	if (IsValid(Quest))
		AvailableQuests.AddUnique(Quest);
}

TArray<TObjectPtr<UQuestStatus>> UQuestService::GetQuestStatus(FGameplayTag QuestTag)
{
	TArray<TObjectPtr<UQuestStatus>> Quests;
	if (const auto* Quest = ActiveQuests.Find(QuestTag))
		Quests.Add(*Quest);
	return Quests;
}

TArray<TObjectPtr<UQuestStatus>> UQuestService::GetQuestStatusByDestination(FGameplayTag Destination)
{
	TArray<TObjectPtr<UQuestStatus>> Quests;
	for (const auto& Entry : ActiveQuests)
	{
		const auto* Delivery = Cast<UDeliveryMissionStatus>(Entry.Value->MissionStatus);
		if (Delivery && Delivery->GetDestinationTag() == Destination)
			Quests.Add(Entry.Value);
		const auto* Travel = Cast<UTravelMissionStatus>(Entry.Value->MissionStatus);
		if (Travel && Travel->GetDestinationTag().IsValid() && Travel->GetDestinationTag() == Destination)
			Quests.Add(Entry.Value);
	}
	return Quests;
}

TArray<TObjectPtr<UQuestStatus>> UQuestService::GetQuestsStatusByOrigin(FGameplayTag OriginIsland)
{
	TArray<TObjectPtr<UQuestStatus>> Quests;
	for (const auto& Entry : ActiveQuests)
	{
		if (Entry.Value->StartIslandTag == OriginIsland)
			Quests.Add(Entry.Value);
	}
	
	return Quests;
}

TArray<TObjectPtr<UQuestData>> UQuestService::GetAvailableQuestsByStartLocation(FGameplayTag StartLocation)
{
	TArray<TObjectPtr<UQuestData>> Quests;
	
	for (const auto Entry : AvailableQuests)
	{
		if (Entry->StartLocationTag == StartLocation)
			Quests.Add(Entry);
	}
	
	return Quests;
}
