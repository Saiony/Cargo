// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "InputDisplayWidget.h"
#include "ShipBalanceWidget.h"
#include "ShipWeightWidget.h"
#include "CargoMainHUD.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UCargoMainHUD : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInputDisplayWidget> InputDisplayWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UShipWeightWidget> ShipWeightWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UShipBalanceWidget> ShipBalanceWidget;
	
	virtual void NativeConstruct() override;
	
};
