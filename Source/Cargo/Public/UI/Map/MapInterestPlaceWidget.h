// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MapInterestPlaceWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UMapInterestPlaceWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PinImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText;
	
public:
	void Init(TObjectPtr<UTexture2D> Image, const FText& Title) const;
};
