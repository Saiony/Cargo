// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/Image.h"
#include "ShipBalanceWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UShipBalanceWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> NeedleImg;
	
	UPROPERTY(Interp)
	float ShipRotation = 30;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
	UFUNCTION()
	void HandleBalanceChanged(float NewBalance);
	
	UFUNCTION(BlueprintImplementableEvent)
	void RotateShipIcon(float NewBalance);
};
