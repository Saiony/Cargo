#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GenericButton.generated.h"

class UCommonTextBlock;

UCLASS()
class CARGO_API UGenericButton : public UCommonButtonBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text;

public:
	void SetText(const FText& InText);
};
