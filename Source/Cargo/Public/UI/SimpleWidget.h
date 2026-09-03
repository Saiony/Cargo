// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SimpleWidget.generated.h"

class UWidgetAnimation;

/**
 * Base widget with optional animated visibility transitions.
 */
UCLASS(Abstract, Blueprintable)
class CARGO_API USimpleWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void Show();

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void Hide();

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> VisibilityAnimation;

private:
	UFUNCTION()
	void HandleVisibilityAnimationFinished();

	bool bIsHiding = false;
	bool bIsVisibilityAnimationBound = false;
};
