// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CompassWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UCompassWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	class UImage* CompassImage;

	UPROPERTY(EditAnywhere, Category = "Compass")
	TObjectPtr<APlayerCameraManager> CachedCameraManager;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
