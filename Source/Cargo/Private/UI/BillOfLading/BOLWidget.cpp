// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BillOfLading/BOLWidget.h"

#include "CommonTextBlock.h"
#include "Components/VerticalBox.h"
#include "Mission/MissionStatus.h"
#include "UI/SimpleWidget.h"
#include "UI/Quest/CargoRequirementEntryWidget.h"

void UBOLWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Stars = { Star1, Star2, Star3 };
	
	ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::OnConfirmButtonClicked);
}

void UBOLWidget::Init(const TObjectPtr<UMissionStatus> MissionStatus)
{
	if (!IsValid(MissionStatus))
	{
		return;
	}

	const TObjectPtr<ACargoPlayerState> PlayerState = GetOwningPlayerState<ACargoPlayerState>();
	if (!IsValid(PlayerState))
	{
		return;
	}

	const FMissionReward MissionReward(MissionStatus);
	Show();
	
	ShipNameText->SetText(FText::FromString(PlayerState->GetShipName()));
	CaptainNameText->SetText(FText::FromString(PlayerState->GetCaptainName()));
	
	OriginLocationText->SetText(FText::FromName(MissionReward.StartIslandTag.GetTagName()));
	DestinationLocationText->SetText(FText::FromName(MissionReward.DestinationTag.GetTagName()));
	
	DeliveryTimeText->SetText(FText::FromString("Conceito de tempo nao implementado :("));
	MissingCargoDiscount->SetText(FText::AsNumber(MissionReward.MissingCargoDiscount));
	DamagedCargoDiscount->SetText(FText::AsNumber(MissionReward.DamagedCargoDiscount));
	RecklessNavigationDiscount->SetText(FText::AsNumber(MissionReward.RecklessNavigationDiscount));
	
	BaseRewardText->SetText(FText::AsNumber(MissionReward.BaseReward.Money));
	FinalRewardText->SetText(FText::AsNumber(MissionReward.FinalReward.Money));
	
	//populates delivered/total cargo requirements
	for (const auto& RequirementTuple : MissionReward.DeliveredQuantities)
	{
		const auto CargoStatus = RequirementTuple.Value;
		auto ReqWidget = CreateWidget<UCargoRequirementEntryWidget>(this, CargoRequirementWidgetClass.Get());

		ReqWidget->Init(CargoStatus);
		RequirementsContainer->AddChild(ReqWidget);
	}
	
	for (int i = 0; i < MissionReward.Stars; i++)
	{
		Stars[i]->Show();
	}
}

void UBOLWidget::Show()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UBOLWidget::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UBOLWidget::OnConfirmButtonClicked()
{
	Hide();
}
