// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "UI/Mission/MissionBoardWidget.h"
#include "IslandWidget.generated.h"

class UDialogueOptionButton;
class ACargoIsland;

/**
 * 
 */
UCLASS()
class CARGO_API UIslandWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> OptionsContainer;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DialogueButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MissionBoardButton;	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
	UPROPERTY()
	TObjectPtr<ACargoIsland> Island;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TSubclassOf<UMissionBoardWidget> MissionBoardWidgetClass;
	
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void OnDialogueButtonClicked();
	
	UFUNCTION()
	void OnMissionBoardButtonClicked();
	
	UFUNCTION()
	void OnCloseButtonClicked();
public:
	void Initialize(TObjectPtr<ACargoIsland> IslandRef);	
};
