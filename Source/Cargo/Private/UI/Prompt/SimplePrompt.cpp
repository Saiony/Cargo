// Fill out your copyright notice in the Description page of Project Settings.


#include "SimplePrompt.h"
#include "Components/Button.h"
#include "PrimaryGameLayout.h"

void USimplePrompt::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	AcceptButton->OnClicked.AddDynamic(this, &ThisClass::OnAcceptButtonClicked);
}

void USimplePrompt::Initialize(const FText& InTitle, const FText& InAcceptText, TFunction<void()> InAcceptCallback)
{
	TitleText->SetText(InTitle);
	AcceptText->SetText(InAcceptText);
	AcceptCallback = MoveTemp(InAcceptCallback);
}

void USimplePrompt::OnAcceptButtonClicked()
{
	if (AcceptCallback)
		AcceptCallback();

	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}
