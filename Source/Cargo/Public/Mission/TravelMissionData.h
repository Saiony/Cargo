#pragma once

#include "CoreMinimal.h"
#include "Mission/BaseMissionData.h"
#include "GameplayTagContainer.h"
#include "TravelMissionData.generated.h"

UCLASS()
class CARGO_API UTravelMissionData : public UBaseMissionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Location"))
	FGameplayTag DestinationTag;
};
