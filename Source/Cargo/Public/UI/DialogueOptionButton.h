// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Dialogue/DialogueData.h"
#include "DialogueOptionButton.generated.h"


class UDialogueOptionButton;

UINTERFACE()
class UDialogueOptionListener : public UInterface
{
	GENERATED_BODY()
};

class IDialogueOptionListener
{
	GENERATED_BODY()

public:
	virtual void OnDialogueOptionClicked(UDialogueOptionButton* Button, const int8 Id) = 0;
};

class UCommonTextBlock;
/**
 *
 */
UCLASS()
class CARGO_API UDialogueOptionButton : public UCommonButtonBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text;

	IDialogueOptionListener* Listener = nullptr;

	int8 Id = -1;

	UPROPERTY()
	TObjectPtr<UDialogueData> Dialogue;

	UPROPERTY()
	TObjectPtr<AActor> Instigator;

protected:
	virtual void NativeOnClicked() override;

public:
	void Init(const FText& Title, const int8 InId, IDialogueOptionListener* InListener);
	void Init(const FText& Title, UDialogueData* InDialogue, AActor* InInstigator, IDialogueOptionListener* InListener);
};
