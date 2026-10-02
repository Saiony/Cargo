#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "StoreButton.generated.h"

class UCommonTextBlock;
class UMoneyTextWidget;
class UEconomyService;

UCLASS()
class CARGO_API UStoreButton : public UCommonButtonBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ContainerNameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMoneyTextWidget> MoneyTextWidget;

	int32 Price = 0;
	TWeakObjectPtr<UEconomyService> EconomyService;

	void UpdateButtonStatus();
	void OnMoneyChanged(int32 Money);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void Init(const FText& ContainerName, int32 Price);
};
