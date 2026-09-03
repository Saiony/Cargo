// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SimpleWidget.h"

#include "Animation/WidgetAnimation.h"

void USimpleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (VisibilityAnimation && !bIsVisibilityAnimationBound)
	{
		FWidgetAnimationDynamicEvent AnimationFinishedEvent;
		AnimationFinishedEvent.BindDynamic(this, &ThisClass::HandleVisibilityAnimationFinished);
		BindToAnimationFinished(VisibilityAnimation, AnimationFinishedEvent);
		bIsVisibilityAnimationBound = true;
	}
}

void USimpleWidget::Show()
{
	bIsHiding = false;
	SetVisibility(ESlateVisibility::Visible);

	if (VisibilityAnimation)
	{
		PlayAnimationForward(VisibilityAnimation);
	}
}

void USimpleWidget::Hide()
{
	if (!VisibilityAnimation)
	{
		bIsHiding = false;
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	bIsHiding = true;
	PlayAnimationReverse(VisibilityAnimation);
}

void USimpleWidget::HandleVisibilityAnimationFinished()
{
	if (bIsHiding)
	{
		bIsHiding = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
