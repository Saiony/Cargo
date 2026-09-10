// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "FrogsmithActivatableWidget.h"
#include "CargoInputTextWidget.generated.h"

class UButton;

UINTERFACE()
class UCargoInputTextListener : public UInterface
{
	GENERATED_BODY()
};

class ICargoInputTextListener
{
	GENERATED_BODY()

public:
	virtual void OnCargoInputTextConfirmed(const FString& Text) = 0;
};

class UEditableTextBox;
/**
 * 
 */
UCLASS()
class CARGO_API UCargoInputTextWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TitleText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> InputTextBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TObjectPtr<USoundWave> TypingSound;
	
	UPROPERTY()
	TScriptInterface<ICargoInputTextListener> Listener;
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnActivated() override;
	
	UFUNCTION()
	void OnTextChanged(const FText& Text);
	
	UFUNCTION()
	void OnConfirmButtonClicked();
public:
	void Init(const FString& Title, const FString& PreviewText, TScriptInterface<ICargoInputTextListener> InListener);
	
	void Hide();
};
