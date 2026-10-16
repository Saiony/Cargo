#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GenericButton.generated.h"

class UImage;
class UCommonTextBlock;

UCLASS()
class CARGO_API UGenericButton : public UCommonButtonBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Icon;

	void SetText(const FText& InText) const;	
	void SetImage(UTexture2D* InImage) const;
	
public:
	void Initialize(const FText& InText, UTexture2D* InImage) const;
};
