#include "UI/Generic/GenericButton.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"

void UGenericButton::Initialize(const FText& InText, UTexture2D* InImage) const
{
	SetText(InText);
	SetImage(InImage);
}

void UGenericButton::SetText(const FText& InText) const
{
	Text->SetText(InText);
	Text->SetVisibility(InText.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

void UGenericButton::SetImage(UTexture2D* InImage) const
{
	//not mandatory to have an image, since it's a generic button
	if (!InImage)
	{
		Icon->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	Icon->SetBrushFromTexture(InImage);
	Icon->SetVisibility(ESlateVisibility::Visible);
}
