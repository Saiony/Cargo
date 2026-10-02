#pragma once

#include "CoreMinimal.h"
#include "Island/IslandOptionData.h"
#include "Island/StoreCatalog.h"
#include "IslandStoreOptionData.generated.h"

UCLASS()
class CARGO_API UIslandStoreOptionData : public UIslandOptionData
{
	GENERATED_BODY()

public:
	virtual FText GetButtonText() const override
	{
		return NSLOCTEXT("IslandOptions", "OpenStore", "Abrir Loja");
	}

	UPROPERTY(EditAnywhere, Category="Store")
	TObjectPtr<UStoreCatalog> StoreCatalog;
};
