#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "Components/Button.h"
#include "StoreWidget.generated.h"

class ACargoIsland;
class UStoreCatalog;
class UStoreButton;
class UVerticalBox;

UCLASS()
class CARGO_API UStoreWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Store")
	TSubclassOf<UStoreButton> StoreButtonClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> StoreButtonsContainer;

	UPROPERTY(BlueprintReadOnly, Category="Store")
	TObjectPtr<UStoreCatalog> StoreCatalog;

	UPROPERTY(BlueprintReadOnly, Category="Store")
	TObjectPtr<ACargoIsland> Island;

	void OnStoreButtonClicked(FGameplayTag ContainerId);

	UFUNCTION()
	void OnCloseButtonClicked();
public:
	
	virtual void NativeOnInitialized() override;
	
	void Init(UStoreCatalog* InStoreCatalog, ACargoIsland* InIsland);
};
