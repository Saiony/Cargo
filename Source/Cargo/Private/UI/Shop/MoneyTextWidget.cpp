#include "UI/Shop/MoneyTextWidget.h"

#include "CommonTextBlock.h"

void UMoneyTextWidget::Init(int32 Amount)
{
	MoneyText->SetText(FText::AsNumber(Amount));
}
