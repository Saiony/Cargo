// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "InputDisplayWidget.h"
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
	
	virtual void NativeConstruct() override;
	
};
