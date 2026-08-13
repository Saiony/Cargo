// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "MissionRequirementEntryWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Mission/MissionData.h"
#include "MissionEntryWidget.generated.h"


class UMissionEntryWidget;

UINTERFACE(MinimalAPI)
class UMissionEntryListener : public UInterface
{
	GENERATED_BODY()
};

class IMissionEntryListener
{
	GENERATED_BODY()

public:
	virtual void OnMissionEntryClicked(UMissionEntryWidget* MissionEntryWidget) = 0;
};


class UButton;
/**
 * 
 */
UCLASS()
class CARGO_API UMissionEntryWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DestinationText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> RequirementsContainer;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ExclamationBorder;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ActiveMissionBorder;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	FLinearColor DefaultBorderColor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	FLinearColor HighlightBorderColor;	
	
	UPROPERTY()
	TArray<TObjectPtr<UMissionRequirementEntryWidget>> MissionRequirementEntries;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TSubclassOf<UMissionRequirementEntryWidget> MissionRequirementEntryClass;
	
	IMissionEntryListener* Listener;	

	UFUNCTION()
	void OnButtonClicked();	
	
	void InitializeForActiveMission() const;
	
public:
	UPROPERTY()
	TObjectPtr<UMissionData> MissionData;
	
	bool IsActiveMission;
	
	void Highlight();
	
	void Fade();

	virtual void NativeOnInitialized() override;
	
	void Initialize(TObjectPtr<UMissionData> InMissionData, IMissionEntryListener* InListener, bool InIsActiveQuest);	
};
