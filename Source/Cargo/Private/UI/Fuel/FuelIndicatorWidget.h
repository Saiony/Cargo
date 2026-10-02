// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "FuelIndicatorWidget.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class CARGO_API UFuelIndicatorWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> FuelBar;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnFuelChanged(float CurrentFuel, float MaxFuel);
};
