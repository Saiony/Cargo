// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Dialogue/DialogueData.h"
#include "DialogueOptionButton.generated.h"

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
	
public:
	void Init(const FCargoDialogueChoice& Choice);
};
