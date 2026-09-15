// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "UI/DialogueOptionButton.h"
#include "UI/Dialogue/QuestDialogueOptionButton.h"
#include "UI/Mission/MissionBoardWidget.h"
#include "IslandWidget.generated.h"

class UDialogueOptionButton;
class ACargoIsland;

/**
 *
 */
UCLASS()
class CARGO_API UIslandWidget : public UFrogsmithActivatableWidget, public IDialogueOptionListener, public IQuestDialogueOptionListener
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> OptionsContainer;

	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TSubclassOf<UDialogueOptionButton> DialogueOptionButtonClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TSubclassOf<UQuestDialogueOptionButton> QuestDialogueOptionButtonClass;	

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MissionBoardButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float DialogueOptionsContainerPadding = 10.0f;

	UPROPERTY()
	TObjectPtr<ACargoIsland> Island;

	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TSubclassOf<UMissionBoardWidget> MissionBoardWidgetClass;

	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnMissionBoardButtonClicked();

	UFUNCTION()
	void OnCloseButtonClicked();

	void DrawDialogueButtons();
	void CreateQuestDialogueOptionButton(TObjectPtr<UQuestData> Quest, EQuestDialogueOptionType Type);

	void CreateDialogueOptionButton(const FText& Title, UDialogueData* Dialogue);

public:
	void Initialize(TObjectPtr<ACargoIsland> IslandRef);
	
	virtual void OnQuestDialogueOptionClicked(UQuestDialogueOptionButton* Button) override;
	virtual void OnDialogueOptionClicked(UDialogueOptionButton* Button, const int8 Id) override;
};
