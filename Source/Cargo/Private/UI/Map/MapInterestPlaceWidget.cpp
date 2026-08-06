// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Map/MapInterestPlaceWidget.h"

void UMapInterestPlaceWidget::Initialize(const TObjectPtr<UTexture2D> Image, const FText& Title) const
{
	//PinImage->SetBrushFromTexture(Image);
	TitleText->SetText(Title);
}
