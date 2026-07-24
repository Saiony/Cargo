// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueOptionButton.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/VerticalBox.h"
#include "Dialogue/DialogueData.h"

#include "DIalogueWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDialoguefinished, UDialogueData*);

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
	FOnDialoguefinished OnDialogueFinishedDelegate;
	
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	
	UFUNCTION(BlueprintCallable, Category="Arcade")
	void InitializeDialogue(UDialogueData* InDialogueDefinition);

	void SetInstigator(AActor* InInstigator) { CurrentInstigator = InInstigator; }

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	void SetupAndPlayDialogue();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnInputActionContinue();
	
private:
	void ShowNextLine();
	void UpdateVisualsForLine(const FARCDialogueLine& Line);
	void OnDialogueFinished();
	void DisplayChoices();
	void OnChoiceSelected(int buttonIndex);
	void Hide();

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
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> OptionsVerticalBox;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowAnimation;
	
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> HideAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> LineTransitionAnimation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	float ChildrenPadding = 15.0f;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo")
	int32 CharsPerSound = 2;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TSubclassOf<UDialogueOptionButton> DialogueOptionButtonClass;
	
private:
	UPROPERTY()
	TObjectPtr<UDialogueData> CurrentDialogueData;

	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentInstigator;
	
	int32 CurrentLineIndex = -1;
	FText FullLineText;
	float CurrentCharCount;
	bool bIsTyping;
	
	int32 LastCharsShown = 0;
};
