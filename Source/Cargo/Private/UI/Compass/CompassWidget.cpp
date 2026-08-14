// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Compass/CompassWidget.h"

#include "Components/Image.h"

void UCompassWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CachedCameraManager = GetOwningPlayer()->PlayerCameraManager;
}

void UCompassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const auto CameraYaw = CachedCameraManager->GetCameraRotation().Yaw;
	CompassImage->SetRenderTransformAngle(-CameraYaw);
}
