#include "Subsystem/CargoTweenSubsystem.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

const FName UCargoTweenSubsystem::ShakeTargetTag(TEXT("ShakeTarget"));

namespace
{
	void ApplyRotationOffset(USceneComponent* Component, const FRotator& PreviousOffset,
		const FRotator& NewOffset)
	{
		if (!IsValid(Component))
			return;

		const FQuat Rotation = Component->GetRelativeRotation().Quaternion() *
			PreviousOffset.Quaternion().Inverse() * NewOffset.Quaternion();
		Component->SetRelativeRotation(Rotation);
	}
}

void UCargoTweenSubsystem::DoShake(AActor* Actor, float Intensity, float Duration)
{
	if (!IsValid(Actor) || Actor->GetWorld() != GetWorld() ||
		!FMath::IsFinite(Intensity) || !FMath::IsFinite(Duration) || Intensity <= 0.f || Duration <= 0.f)
		return;

	USceneComponent* Target = Actor->FindComponentByTag<USceneComponent>(ShakeTargetTag);
	if (!Target)
		Target = Actor->GetRootComponent();
	if (!IsValid(Target))
		return;

	const TWeakObjectPtr<AActor> Key(Actor);
	FShake& Shake = Shakes.FindOrAdd(Key);
	USceneComponent* PreviousTarget = Shake.Target.Get();
	const FRotator PreviousOffset = Shake.RotationOffset;
	Shake.Target = Target;
	Shake.RotationOffset = FRotator::ZeroRotator;
	Shake.Intensity = Intensity;
	Shake.Generation = ++NextShakeGeneration;
	const uint64 Generation = Shake.Generation;

	const double StartTime = GetWorld()->GetTimeSeconds();
	const float Seed = FMath::FRandRange(0.f, 10000.f);
	GetWorld()->GetTimerManager().SetTimer(Shake.Timer, FTimerDelegate::CreateWeakLambda(this,
		[this, Key, Generation, Duration, StartTime, Seed]()
		{
			FShake* Active = Shakes.Find(Key);
			if (!Active || Active->Generation != Generation)
				return;

			USceneComponent* Component = Active->Target.Get();
			const float Elapsed = GetWorld()->GetTimeSeconds() - StartTime;
			if (!Key.IsValid() || !Component || Elapsed >= Duration)
			{
				const TWeakObjectPtr<AActor> ShakeKey = Key;
				FTimerHandle TimerToClear = Active->Timer;
				const FRotator PreviousOffset = Active->RotationOffset;
				UWorld* const World = GetWorld();
				Shakes.Remove(ShakeKey);
				World->GetTimerManager().ClearTimer(TimerToClear);
				ApplyRotationOffset(Component, PreviousOffset, FRotator::ZeroRotator);
				return;
			}

			const float Time = Seed + Elapsed * 18.f;
			const float Envelope = 1.f - Elapsed / Duration;
			const FRotator NewOffset(
				FMath::PerlinNoise1D(Time) * Active->Intensity * Envelope,
				FMath::PerlinNoise1D(Time + 37.1f) * Active->Intensity * Envelope,
				FMath::PerlinNoise1D(Time + 91.7f) * Active->Intensity * Envelope);

			// Replace only this shake's rotation offset, preserving other rotation changes.
			const FRotator PreviousOffset = Active->RotationOffset;
			Active->RotationOffset = NewOffset;
			ApplyRotationOffset(Component, PreviousOffset, NewOffset);
		}), 1.f / 60.f, true);

	if (PreviousTarget)
		ApplyRotationOffset(PreviousTarget, PreviousOffset, FRotator::ZeroRotator);
}

void UCargoTweenSubsystem::SetShakeIntensity(AActor* Actor, float Intensity)
{
	if (!IsValid(Actor) || !FMath::IsFinite(Intensity) || Intensity < 0.f)
		return;

	if (FShake* Shake = Shakes.Find(TWeakObjectPtr<AActor>(Actor)))
		Shake->Intensity = Intensity;
}

void UCargoTweenSubsystem::StopShake(AActor* Actor)
{
	if (!IsValid(Actor))
		return;

	const TWeakObjectPtr<AActor> Key(Actor);
	FShake* Shake = Shakes.Find(Key);
	if (!Shake)
		return;

	USceneComponent* Component = Shake->Target.Get();
	const FRotator PreviousOffset = Shake->RotationOffset;
	GetWorld()->GetTimerManager().ClearTimer(Shake->Timer);
	Shakes.Remove(Key);
	ApplyRotationOffset(Component, PreviousOffset, FRotator::ZeroRotator);
}

void UCargoTweenSubsystem::Deinitialize()
{
	for (auto& Pair : Shakes)
	{
		GetWorld()->GetTimerManager().ClearTimer(Pair.Value.Timer);
		if (USceneComponent* Target = Pair.Value.Target.Get())
			ApplyRotationOffset(Target, Pair.Value.RotationOffset, FRotator::ZeroRotator);
	}
	Shakes.Empty();
	Super::Deinitialize();
}
