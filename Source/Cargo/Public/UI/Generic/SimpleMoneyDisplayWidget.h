// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Image.h"
#include "SimpleMoneyDisplayWidget.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class CARGO_API USimpleMoneyDisplayWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> CoinImage;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PriceText;
	
public:
	void Initialize(int32 Price) const;
};
