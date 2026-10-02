#include "UI/Shop/StoreButton.h"

#include "CommonTextBlock.h"
#include "CargoGameMode.h"
#include "Services/EconomyService.h"
#include "UI/Shop/MoneyTextWidget.h"

void UStoreButton::NativeConstruct()
{
	Super::NativeConstruct();
	EconomyService = ACargoGameMode::Get(this)->EconomyService;
	EconomyService->OnMoneyChanged.AddUObject(this, &ThisClass::OnMoneyChanged);
	UpdateButtonStatus();
}

void UStoreButton::NativeDestruct()
{
	if (EconomyService.IsValid())
		EconomyService->OnMoneyChanged.RemoveAll(this);

	Super::NativeDestruct();
}

void UStoreButton::Init(const FText& ContainerName, int32 Price)
{
	this->Price = Price;
	ContainerNameText->SetText(ContainerName);
	MoneyTextWidget->Init(Price);
	UpdateButtonStatus();
}

void UStoreButton::UpdateButtonStatus()
{
	SetIsEnabled(ACargoGameMode::Get(this)->EconomyService->HasMoney(Price));
}

void UStoreButton::OnMoneyChanged(int32 Money)
{
	UpdateButtonStatus();
}
