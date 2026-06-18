// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Dialogue/DialogueData.h"

#include "DIalogueWidget.generated.h"

class UCommonTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CARGO_API UDIalogueWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	
	UFUNCTION(BlueprintCallable, Category="Arcade")
	void InitializeDialogue(UDialogueData* InDialogueDefinition);

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	void SetupAndPlayDialogue();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnInputActionContinue();
	
private:
	void ShowNextLine();
	void UpdateVisualsForLine(const FARCDialogueLine& Line);
	void RequestClose();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Arcade", meta=(AllowPrivateAccess))
	FDataTableRowHandle SkipDialogueInputActionData;
	FUIActionBindingHandle SkipDialogueInputHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Arcade", meta=(AllowPrivateAccess))
	float TypewriterSpeed = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Arcade", meta=(AllowPrivateAccess))
	TObjectPtr<USoundBase> DialogueAudio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Arcade", meta=(AllowPrivateAccess))
	TObjectPtr<USoundConcurrency> DialogueConcurrency;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImageLeftPortrait;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImageRightPortrait;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> TextDialogue;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowAnimation;
	
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> HideAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> LineTransitionAnimation;
	
private:
	UPROPERTY()
	TObjectPtr<UDialogueData> DialogueDefinition;
	
	int32 CurrentLineIndex = -1;
	FText FullLineText;
	float CurrentCharCount;
	bool bIsTyping;
};
