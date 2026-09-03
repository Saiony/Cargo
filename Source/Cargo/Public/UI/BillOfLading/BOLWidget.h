// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Button.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "Mission/MissionReward.h"
#include "BOLWidget.generated.h"

class USimpleWidget;
class UMissionStatus;
class UVerticalBox;
class UCargoRequirementEntryWidget;
class UCommonTextBlock;
/**
 * Bill of Lading widget
 */
UCLASS()
class CARGO_API UBOLWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ShipNameText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CaptainNameText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OriginLocationText; 
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DestinationLocationText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DeliveryTimeText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> RecklessNavigationDiscount;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> MissingCargoDiscount;	
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DamagedCargoDiscount;	
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> BaseRewardText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> FinalRewardText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> RequirementsContainer;
	
	/*stars*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USimpleWidget> Star1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USimpleWidget> Star2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USimpleWidget> Star3;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<USimpleWidget>> Stars;
	
	/*class references*/
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TSoftClassPtr<UCargoRequirementEntryWidget> CargoRequirementWidgetClass;	
	
protected:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void OnConfirmButtonClicked();
public:
	void Init(TObjectPtr<ACargoPlayerState> PlayerState, const FMissionReward& MissionReward);	
	void Show();	
	void Hide();
};
