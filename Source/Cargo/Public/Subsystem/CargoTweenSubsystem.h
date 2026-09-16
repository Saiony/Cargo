#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CargoTweenSubsystem.generated.h"

class AActor;
class USceneComponent;

UCLASS()
class CARGO_API UCargoTweenSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	struct FShake
	{
		FTimerHandle Timer;
		TWeakObjectPtr<USceneComponent> Target;
		FVector Offset = FVector::ZeroVector;
	};

	TMap<TWeakObjectPtr<AActor>, FShake> Shakes;

public:
	// Actors can mark a visual component to shake instead of their collision root.
	static const FName ShakeTargetTag;

	/** Translation only. Intensity is in centimeters; Duration is in seconds. */
	UFUNCTION(BlueprintCallable, Category="Cargo|Tween")
	void DoShake(AActor* Actor, float Intensity, float Duration);

	virtual void Deinitialize() override;
};
