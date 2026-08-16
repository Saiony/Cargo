// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "MoneyWidget.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class CARGO_API UMoneyWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> MoneyText;
	
protected:
	virtual void NativeOnInitialized() override;
	
public:
	void OnMoneyChanged(int32 Amount);
};
