// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestsTrackerWidget.h"

#include "CargoGameMode.h"
#include "Components/VerticalBox.h"
#include "UI/Quest/QuestEntryWidget.h"
#include "Quest/QuestStatus.h"

void UQuestsTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();	
	
	ACargoGameMode* GM = ACargoGameMode::Get(this);
	GM->ActiveQuestsDelegate.AddUObject(this, &ThisClass::OnActiveQuestsChanged);
	GM->OnQuestProgressUpdatedDelegate.AddUObject(this, &ThisClass::OnQuestProgressUpdated);
	GM->QuestCompletedDelegate.AddUObject(this, &ThisClass::OnQuestCompleted);
}

void UQuestsTrackerWidget::OnActiveQuestsChanged(const FActiveQuestsMap& ActiveQuests)
{
	QuestContainer->ClearChildren();
	QuestWidgets.Empty();

	for (const auto& [QuestTag, Status] : ActiveQuests)
	{
		UQuestEntryWidget* Entry = CreateWidget<UQuestEntryWidget>(GetOwningPlayer(), QuestEntryWidgetClass);

		Entry->Init(QuestTag, Status->OriginalQuestData);

		QuestContainer->AddChild(Entry);
		QuestWidgets.Add(QuestTag, Entry);
	}
}

void UQuestsTrackerWidget::OnQuestProgressUpdated(TObjectPtr<UQuestStatus> QuestStatus, FGameplayTag CargoType, int32 NewDeliveredAmount)
{
	if (QuestStatus && QuestWidgets.Contains(QuestStatus->QuestTag))
	{
		QuestWidgets[QuestStatus->QuestTag]->UpdateRequirement(CargoType, NewDeliveredAmount);
	}
}

void UQuestsTrackerWidget::OnQuestCompleted(TObjectPtr<UQuestStatus> QuestStatus)
{
	if (QuestStatus && QuestWidgets.Contains(QuestStatus->QuestTag))
	{
		UQuestEntryWidget* Entry = QuestWidgets[QuestStatus->QuestTag];
		Entry->OnAnimationFinished.AddDynamic(this, &ThisClass::OnQuestEntryAnimationFinished);
		Entry->Complete();
	}
}

void UQuestsTrackerWidget::OnQuestEntryAnimationFinished(UQuestEntryWidget* Widget)
{
	if (Widget->GetQuestTag().IsValid())
	{
		QuestWidgets.Remove(Widget->GetQuestTag());
	}

	QuestContainer->RemoveChild(Widget);
}
