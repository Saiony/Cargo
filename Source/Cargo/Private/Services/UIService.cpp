// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/UIService.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "PrimaryGameLayout.h"
#include "DeveloperSettings/CargoSettings.h"
#include "TagDeclaration/UITypes.h"


class UPrimaryGameLayout;

void UUIService::FadeIn(float Duration, TFunction<void()> Callback)
{
	const auto CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	CameraManager->StartCameraFade(
		0.f, 1.f, Duration, FLinearColor::Black, false, true);

	GetWorld()->GetTimerManager().SetTimer(
		FadeTimer,
		FTimerDelegate::CreateWeakLambda(this, [Callback = MoveTemp(Callback)]()
		{
			if (Callback)
				Callback();
		}),
		Duration,
		false);
}

void UUIService::FadeOut(float Duration, TFunction<void()> Callback)
{
	const auto CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	CameraManager->StartCameraFade(
		1.f, 0.f, Duration, FLinearColor::Black, false, false);

	GetWorld()->GetTimerManager().SetTimer(
		FadeTimer,
		FTimerDelegate::CreateWeakLambda(this, [Callback = MoveTemp(Callback)]()
		{
			if (Callback)
				Callback();
		}),
		Duration,
		false);
}

UUIService::UUIService()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UUIService::Boot(FOnServiceBooted OnBootFinished)
{
	Super::Boot(OnBootFinished);	
	
	OnBootFinished.Execute();
}

UFrogsmithActivatableWidget* UUIService::ShowWidget(const TSubclassOf<UFrogsmithActivatableWidget> WidgetClass) const
{
	if (WidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UI Service: Widget class can't be null"));
		return nullptr;
	}

	UPrimaryGameLayout* Layout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	
	if (!Layout)
	{
		UE_LOG(LogTemp, Error, TEXT("UI Service: PrimaryGameLayout is unavailable"));
		return nullptr;
	}
	
	const auto Widget = Layout->PushWidgetToLayerStack<UFrogsmithActivatableWidget>(TAG_UI_Layer_Menu, WidgetClass);
	if (!Widget)
	{
		UE_LOG(LogTemp, Error, TEXT("UI Service: Failed to push widget to layer stack"));
		return nullptr;
	}
	
	return Widget;
}

UCargoInputTextWidget* UUIService::ShowInputTextWidget() const
{
	const auto InputTextWidgetClass = GetDefault<UCargoSettings>()->InputTextWidgetClass.LoadSynchronous();
	const auto InputTextWidget = ShowWidget(InputTextWidgetClass);
	
	return Cast<UCargoInputTextWidget>(InputTextWidget);
}
