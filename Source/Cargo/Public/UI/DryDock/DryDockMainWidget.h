// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "DryDockMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UDryDockMainWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CategoryBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> UpgradesBox;
	
protected:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void OnCloseButtonClicked();
	
	void Hide();
};
