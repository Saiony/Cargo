// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "FrogsmithActivatableWidget.h"
#include "Templates/Function.h"
#include "SimplePrompt.generated.h"


class UButton;
/**
 * 
 */
UCLASS()
class CARGO_API USimplePrompt : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> TitleText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UButton> AcceptButton;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> AcceptText;

	TFunction<void()> AcceptCallback;

	UFUNCTION()
	void OnAcceptButtonClicked();
	
protected:
	virtual void NativeOnInitialized() override;
	
public:
	void Initialize(const FText& InTitle, const FText& InAcceptText, TFunction<void()> InAcceptCallback);
};
