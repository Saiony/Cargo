// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ShipWeightWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UShipWeightWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentWeightText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxWeightText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage;
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor SuperLightColor;
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor LightColor;
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor NormalColor;
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor HeavyColor;
	
	UPROPERTY(EditDefaultsOnly)
	FLinearColor SuperHeavyColor;
	
	
protected:	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void HandleWeightChanged(float NewCurrentWeight, float MaxWeight);
};
