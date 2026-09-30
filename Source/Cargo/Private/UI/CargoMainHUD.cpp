// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CargoMainHUD.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Widgets/SViewport.h"

void UCargoMainHUD::SetEditMode(bool bInEditMode)
{
	InputConfig = EFrogsmithWidgetInputMode::Game;
	GameMouseCaptureMode = bInEditMode ? EMouseCaptureMode::NoCapture : EMouseCaptureMode::CapturePermanently;
	bShowMouseCursor = bInEditMode;

	UCommonUIActionRouterBase* ActionRouter = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(GetOwningPlayer()->GetLocalPlayer());
	if (ActionRouter->IsWidgetInActiveRoot(this))
	{
		if (bInEditMode)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			GetOwningPlayer()->SetInputMode(InputMode);
		}
		else
		{
			GetOwningPlayer()->SetInputMode(FInputModeGameOnly());
		}

		ActionRouter->SetActiveUIInputConfig(GetDesiredInputConfig().GetValue(), this);
	}
}

void UCargoMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
	auto& OnActiveInputConfigChanged = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(GetOwningPlayer()->GetLocalPlayer())->OnActiveInputConfigChanged();
	OnActiveInputConfigChanged.RemoveAll(this);
	OnActiveInputConfigChanged.AddUObject(this, &ThisClass::HandleActiveInputConfigChanged);
}

void UCargoMainHUD::HandleActiveInputConfigChanged(FUIInputConfig NewConfig)
{
	UCommonUIActionRouterBase* ActionRouter = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(GetOwningPlayer()->GetLocalPlayer());
	if (!ActionRouter->IsWidgetInActiveRoot(this) || NewConfig != GetDesiredInputConfig().GetValue())
	{
		return;
	}

	if (GameMouseCaptureMode == EMouseCaptureMode::NoCapture)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(StaticCastSharedPtr<SWidget>(GetOwningPlayer()->GetLocalPlayer()->ViewportClient->GetGameViewportWidget()));
		InputMode.SetHideCursorDuringCapture(false);
		GetOwningPlayer()->SetInputMode(InputMode);
	}
	else
	{
		GetOwningPlayer()->SetInputMode(FInputModeGameOnly());
	}
}
