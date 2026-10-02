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
		FRotator RotationOffset = FRotator::ZeroRotator;
		float Intensity = 0.f;
		uint64 Generation = 0;
	};

	TMap<TWeakObjectPtr<AActor>, FShake> Shakes;
	uint64 NextShakeGeneration = 0;

public:
	// Actors can mark a visual component to shake instead of their collision root.
	static const FName ShakeTargetTag;

	/** Applies rotational shake. Intensity is in degrees; Duration is in seconds. */
	UFUNCTION(BlueprintCallable, Category="Cargo|Tween")
	void DoShake(AActor* Actor, float Intensity, float Duration);

	UFUNCTION(BlueprintCallable, Category="Cargo|Tween")
	void StopShake(AActor* Actor);

	void SetShakeIntensity(AActor* Actor, float Intensity);

	virtual void Deinitialize() override;
};
