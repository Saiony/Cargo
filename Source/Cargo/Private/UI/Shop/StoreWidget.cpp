#include "UI/Shop/StoreWidget.h"

#include "CargoGameMode.h"
#include "PrimaryGameLayout.h"
#include "Components/VerticalBox.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Island/CargoIsland.h"
#include "Island/StoreCatalog.h"
#include "Port/CargoPort.h"
#include "Services/EconomyService.h"
#include "UI/Shop/StoreButton.h"

void UStoreWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CloseButton->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
}

void UStoreWidget::Init(UStoreCatalog* InStoreCatalog, ACargoIsland* InIsland)
{
	StoreCatalog = InStoreCatalog;
	Island = InIsland;

	StoreButtonsContainer->ClearChildren();

	const auto Settings = GetDefault<UCargoSettings>();
	const auto ContainerId = StoreCatalog->ContainerId;
	const auto ContainerData = Settings->ContainersMap.FindChecked(ContainerId).LoadSynchronous();
	const auto Button = CreateWidget<UStoreButton>(this, StoreButtonClass);
	Button->Init(ContainerData->DisplayName, ContainerData->BasePrice);
	Button->OnClicked().AddWeakLambda(this, [this, ContainerId]()
	{
		OnStoreButtonClicked(ContainerId);
	});
	StoreButtonsContainer->AddChildToVerticalBox(Button);
}

void UStoreWidget::OnStoreButtonClicked(FGameplayTag ContainerId)
{
	const auto ContainerData = GetDefault<UCargoSettings>()->ContainersMap.FindChecked(ContainerId).LoadSynchronous();
	const auto EconomyService = ACargoGameMode::Get(this)->EconomyService;
	
	if (!EconomyService->RemoveMoney(ContainerData->BasePrice))
		return;

	Island->GetPort()->OpenPortForPickup();
	Island->GetPort()->SpawnSingleContainer(ContainerId);
}

void UStoreWidget::OnCloseButtonClicked()
{
	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}
