// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CargoGameMode.h"
#include "UObject/Object.h"
#include "QuestsTrackerWidget.generated.h"

class UQuestEntryWidget;
class UVerticalBox;
class UQuestStatus;

/**
 * 
 */
UCLASS()
class CARGO_API UQuestsTrackerWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> QuestContainer;

	UPROPERTY(EditDefaultsOnly, Category="Quest")
	TSubclassOf<UQuestEntryWidget> QuestEntryWidgetClass;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UQuestEntryWidget>> QuestWidgets;

	void OnActiveQuestsChanged(const FActiveQuestsMap& ActiveQuests);	
	void OnQuestProgressUpdated(TObjectPtr<UQuestStatus> QuestStatus, FGameplayTag CargoType, int32 NewDeliveredAmount);
	void OnQuestCompleted(TObjectPtr<UQuestStatus> QuestStatus);

	UFUNCTION()
	void OnQuestEntryAnimationFinished(UQuestEntryWidget* Widget);
	
protected:
	virtual void NativeConstruct() override;
};
