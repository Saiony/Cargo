#pragma once

#include "CoreMinimal.h"
#include "Mission/BaseMissionStatus.h"
#include "GameplayTagContainer.h"
#include "TravelMissionStatus.generated.h"

class UTravelMissionData;

UCLASS()
class CARGO_API UTravelMissionStatus : public UBaseMissionStatus
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FGameplayTag DestinationTag;

public:
	void Initialize(const UTravelMissionData* MissionData);
	FGameplayTag GetDestinationTag() const { return DestinationTag; }
};
