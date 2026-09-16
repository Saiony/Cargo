#include "Subsystem/CargoTweenSubsystem.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

const FName UCargoTweenSubsystem::ShakeTargetTag(TEXT("ShakeTarget"));

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
	if (USceneComponent* PreviousTarget = Shake.Target.Get())
		PreviousTarget->SetRelativeLocation(PreviousTarget->GetRelativeLocation() - Shake.Offset);
	Shake.Target = Target;
	Shake.Offset = FVector::ZeroVector;

	const double StartTime = GetWorld()->GetTimeSeconds();
	const float Seed = FMath::FRandRange(0.f, 10000.f);
	GetWorld()->GetTimerManager().SetTimer(Shake.Timer, FTimerDelegate::CreateWeakLambda(this,
		[this, Key, Intensity, Duration, StartTime, Seed]()
		{
			FShake& Active = Shakes.FindChecked(Key);
			USceneComponent* Component = Active.Target.Get();
			const float Elapsed = GetWorld()->GetTimeSeconds() - StartTime;
			if (!Key.IsValid() || !Component || Elapsed >= Duration)
			{
				if (Component)
					Component->SetRelativeLocation(Component->GetRelativeLocation() - Active.Offset);
				GetWorld()->GetTimerManager().ClearTimer(Active.Timer);
				Shakes.Remove(Key);
				return;
			}

			const float Time = Seed + Elapsed * 18.f;
			const FVector Noise(FMath::PerlinNoise1D(Time),
				FMath::PerlinNoise1D(Time + 37.1f), FMath::PerlinNoise1D(Time + 91.7f));
			const FVector Offset = Noise * Intensity * (1.f - Elapsed / Duration);
			// Apply only the change in offset, preserving movement from other systems.
			Component->SetRelativeLocation(Component->GetRelativeLocation() + Offset - Active.Offset);
			Active.Offset = Offset;
		}), 1.f / 60.f, true);
}

void UCargoTweenSubsystem::Deinitialize()
{
	for (auto& Pair : Shakes)
	{
		GetWorld()->GetTimerManager().ClearTimer(Pair.Value.Timer);
		if (USceneComponent* Target = Pair.Value.Target.Get())
			Target->SetRelativeLocation(Target->GetRelativeLocation() - Pair.Value.Offset);
	}
	Shakes.Empty();
	Super::Deinitialize();
}
