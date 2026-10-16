// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Image.h"
#include "CommonTextBlock.h"
#include "UpgradeInfoSingle.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UUpgradeInfoSingle : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AttributeIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> AttributeName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> AttributeValue;

public:
	void Initialize(const FText& Name, UTexture2D* Icon, float Value);
};
