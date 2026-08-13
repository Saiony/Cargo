// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "MissionEntryWidget.h"
#include "Island/CargoIsland.h"
#include "Mission/MissionsDatabase.h"
#include "MissionBoardWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UMissionBoardWidget : public UFrogsmithActivatableWidget, public IMissionEntryListener
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MissionsContainer;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AcceptMissionButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DeliverMissionButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton2;
	
	UPROPERTY()
	TArray<TObjectPtr<UMissionEntryWidget>> MissionEntryWidgets;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TSubclassOf<UMissionEntryWidget> MissionEntryClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TSubclassOf<UMissionEntryWidget> ActiveMissionEntryClass;	
	
	UPROPERTY()
	TObjectPtr<UMissionEntryWidget> SelectedWidget;
	
	UPROPERTY()
	TObjectPtr<ACargoIsland> InstigatorIsland;

	UFUNCTION()
	void OnAcceptMissionButtonClicked();
	
	UFUNCTION()
	void OnDeliverMissionButtonClicked();
	
	void ShowAvailableMissions(FMissions* MissionsData);
	
	void ShowActiveMissions();
	
	UFUNCTION()
	void OnCloseButtonClicked();
	
	void ClearBoard();
	
	void CloseWidget();
public:
	virtual void NativeOnInitialized() override;
	
	void Initialize(FMissions* MissionData, TObjectPtr<ACargoIsland> InInstigatorIsland);
	
	virtual void OnMissionEntryClicked(UMissionEntryWidget* MissionEntryWidget) override;
};
