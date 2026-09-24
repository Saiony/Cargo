#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MoneyTextWidget.generated.h"

class UImage;
class UCommonTextBlock;

UCLASS()
class CARGO_API UMoneyTextWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> MoneyImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> MoneyText;

public:
	UFUNCTION(BlueprintCallable, Category="UI")
	void Init(int32 Amount);
};
