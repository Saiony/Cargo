#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CargoLevelConfig.generated.h"

class APlayerStart;

UCLASS()
class CARGO_API ACargoLevelConfig : public AActor
{
	GENERATED_BODY()

public:
	APlayerStart* GetPlayerStart() const { return PlayerStart; }

private:
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<APlayerStart> PlayerStart;
};
