// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActionWidget.h"
#include "FrogsmithActivatableWidget.h"
#include "Interaction/CargoInteractable.h"
#include "InputDisplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UInputDisplayWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActionWidget> InteractionWidget;
	
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void HandleInteractableChanged(TScriptInterface<ICargoInteractable> NewInteractable);
};
