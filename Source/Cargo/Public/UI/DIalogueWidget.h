#pragma once

#include "CoreMinimal.h"
#include "DialogueOptionButton.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/VerticalBox.h"
#include "Dialogue/DialogueData.h"
#include "Dialogue/DialogueText.h"

#include "DIalogueWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDialoguefinished, UDialogueData*);

class URichTextBlock;
class UImage;

UCLASS()
class CARGO_API UDIalogueWidget : public UFrogsmithActivatableWidget, public IDialogueOptionListener
{
	GENERATED_BODY()
	
public:
	FOnDialoguefinished OnDialogueFinishedDelegate;
	
	UFUNCTION(BlueprintCallable, Category="Arcade")
	void InitializeDialogue(UDialogueData* InDialogueDefinition);

	void SetInstigator(AActor* InInstigator) { CurrentInstigator = InInstigator; }

	
	void SetupAndPlayDialogue();
	
	virtual void OnDialogueOptionClicked(UDialogueOptionButton* Button, const int8 Id) override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;	
protected:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	
	virtual void NativeOnActivated() override;
	
	virtual void NativeOnDeactivated() override;

	UFUNCTION(BlueprintCallable)
	void OnInputActionContinue();
	
private:
	void ShowNextLine();
	void UpdateVisualsForLine(const FARCDialogueLine& Line);
	void OnDialogueFinished();
	void DisplayChoices();
	void Hide();
	void FinishHide();
	void PrepareLineText(const FText& Text);
	void UpdatePlayerDataStyle();
	

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
	TObjectPtr<URichTextBlock> TextDialogue;
	
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
	FDialogueText FormattedLine;
	float CurrentCharCount;
	bool bIsTyping;
	
	int32 LastCharsShown = 0;
};
