// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BillOfLading/BOLWidget.h"

#include "CommonTextBlock.h"
#include "Components/VerticalBox.h"
#include "Mission/MissionStatus.h"
#include "PrimaryGameLayout.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SimpleWidget.h"
#include "UI/Quest/CargoRequirementEntryWidget.h"

void UBOLWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Stars = { Star1, Star2, Star3 };
	
	ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::OnConfirmButtonClicked);
}

void UBOLWidget::NativeOnActivated()
{
	Super::NativeOnActivated();	
	
	RequirementsContainer->ClearChildren(); //important because unreal reuses widgets
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
	
	ShipNameText->SetText(FText::FromString(PlayerState->GetShipName()));
	CaptainNameText->SetText(FText::FromString(PlayerState->GetCaptainName()));
	
	OriginLocationText->SetText(FText::FromName(MissionReward.StartIslandTag.GetTagName()));
	DestinationLocationText->SetText(FText::FromName(MissionReward.DestinationTag.GetTagName()));
	
	DeliveryTimeText->SetText(FText::FromString("-"));
	MissingCargoDiscount->SetText(FText::AsNumber(-MissionReward.MissingCargoDiscount));
	DamagedCargoDiscount->SetText(FText::AsNumber(-MissionReward.DamagedCargoDiscount));
	RecklessNavigationDiscount->SetText(FText::AsNumber(-MissionReward.RecklessNavigationDiscount));
	
	BaseRewardText->SetText(FText::AsNumber(MissionReward.BaseReward.Money));
	FinalRewardText->SetText(FText::AsNumber(MissionReward.FinalReward.Money));		
	
	//populates delivered/total cargo requirements
	for (const auto& RequirementTuple : MissionReward.DeliveredQuantities)
	{
		const auto CargoStatus = RequirementTuple.Value;
		auto ReqWidget = CreateWidget<UCargoRequirementEntryWidget>(this, CargoRequirementWidgetClass);

		ReqWidget->Init(CargoStatus);
		RequirementsContainer->AddChild(ReqWidget);
	}	
	
	EarnedStars = MissionReward.Stars;
	UE_LOG(LogTemp, Log, TEXT("BOLWidget: Earned %d stars"), EarnedStars);
	Show();
}

void UBOLWidget::Show()
{
	FWidgetAnimationDynamicEvent FinishedEvent;
	FinishedEvent.BindDynamic(this, &ThisClass::OnShowAnimationFinished);

	BindToAnimationFinished(ShowAnimation, FinishedEvent);	
	PlayAnimation(ShowAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, true);	
}

void UBOLWidget::OnShowAnimationFinished()
{
	ShowStars(EarnedStars);
}
void UBOLWidget::Hide()
{
	if (UPrimaryGameLayout* Layout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this))
	{
		Layout->FindAndRemoveWidgetFromLayer(this);
		return;
	}

	RemoveFromParent();
}

void UBOLWidget::OnConfirmButtonClicked()
{
	FWidgetAnimationDynamicEvent FinishedEvent;
	FinishedEvent.BindDynamic(this, &ThisClass::OnFinishAnimationFinished);
	
	BindToAnimationFinished(FinishAnimation, FinishedEvent);
	PlayAnimation(FinishAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, true);
}

void UBOLWidget::OnFinishAnimationFinished()
{
	Hide();
}

void UBOLWidget::ShowStars(int8 TotalStars)
{
	FTimerDelegate Delegate;

	Delegate.BindWeakLambda(this, [this, Index = 0, TotalStars]() mutable
	{
		if (Index >= TotalStars || Index >= Stars.Num())
		{
			GetWorld()->GetTimerManager().ClearTimer(StarsTimerHandle);
			return;
		}

		Stars[Index]->Show();
		UGameplayStatics::PlaySound2D(this, StarsSound);
		Index++;
	});

	GetWorld()->GetTimerManager().SetTimer(StarsTimerHandle, Delegate, StarsInterval, true);
}