#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Triggers/PlayerEnterTrigger.h"
#include "GameplayEventTrigger.generated.h"

UCLASS()
class CARGO_API AGameplayEventTrigger : public APlayerEnterTrigger
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo", meta=(Categories="InGameEvent"))
	FGameplayTag GameplayEvent = FGameplayTag::RequestGameplayTag(TEXT("InGameEvent.SawSkullGraffiti"));

protected:
	virtual void OnPlayerEnter() override;
};
