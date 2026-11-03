// Copyright Epic Games, Inc. All Rights Reserved.

#include "Whale/Whale.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CargoCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Whale/AutoDestructible.h"

AWhale::AWhale()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	WhaleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WhaleMesh"));
	WhaleMesh->SetupAttachment(SceneRoot);
	WhaleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WhaleMesh->SetSimulatePhysics(false);
}

void AWhale::InitializeBreach(const FVector& InWaterSurfaceLocation, float InInitialDepth)
{
	WaterSurfaceLocation = InWaterSurfaceLocation;
	InitialDepth = FMath::Max(InInitialDepth, 0.0f);
	bHasWaterSurfaceLocation = true;
	SetActorLocation(WaterSurfaceLocation - FVector(0.0f, 0.0f, InitialDepth));
}

void AWhale::BeginPlay()
{
	Super::BeginPlay();

	if (!bHasWaterSurfaceLocation)
	{
		WaterSurfaceLocation = GetActorLocation() + FVector(0.0f, 0.0f, InitialDepth);
	}
	InitialActorRotation = GetActorRotation();
}

void AWhale::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (DeltaTime <= 0.0f)
	{
		return;
	}

	ElapsedSequenceTime += DeltaTime;
	const float SafeRiseDuration = FMath::Max(RiseDuration, KINDA_SMALL_NUMBER);
	const float RiseDistance = InitialDepth + FMath::Max(BreachHeight, 0.0f);
	const float SurfaceProgress = RiseDistance > KINDA_SMALL_NUMBER
		? FMath::Clamp(InitialDepth / RiseDistance, 0.0f, 1.0f)
		: 0.0f;
	const float SurfaceAlpha = 1.0f - FMath::Sqrt(1.0f - SurfaceProgress);
	const float SurfaceCrossingTime = SurfaceAlpha * SafeRiseDuration;
	if (!bSurfaceImpactTriggered && ElapsedSequenceTime >= SurfaceCrossingTime)
	{
		bSurfaceImpactTriggered = true;
		TriggerSurfaceImpact();
	}

	const float RiseEndTime = SafeRiseDuration;
	const float HoldEndTime = RiseEndTime + PeakHoldDuration;
	const float DescendEndTime = HoldEndTime + FMath::Max(DescendDuration, KINDA_SMALL_NUMBER);
	if (!bToppleStarted && ElapsedSequenceTime >= RiseEndTime)
	{
		bToppleStarted = true;
		TriggerToppleImpact();
	}

	FVector NewLocation = WaterSurfaceLocation;

	if (ElapsedSequenceTime < RiseEndTime)
	{
		const float Alpha = FMath::Clamp(ElapsedSequenceTime / SafeRiseDuration, 0.0f, 1.0f);
		const float EasedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);
		NewLocation.Z = FMath::Lerp(WaterSurfaceLocation.Z - InitialDepth,
			WaterSurfaceLocation.Z + BreachHeight, EasedAlpha);
	}
	else if (ElapsedSequenceTime < HoldEndTime)
	{
		NewLocation.Z = WaterSurfaceLocation.Z + BreachHeight;
	}
	else if (ElapsedSequenceTime < DescendEndTime)
	{
		const float Alpha = (ElapsedSequenceTime - HoldEndTime) / FMath::Max(DescendDuration, KINDA_SMALL_NUMBER);
		const float EasedAlpha = FMath::InterpEaseIn(0.0f, 1.0f, Alpha, 2.0f);
		NewLocation.Z = FMath::Lerp(WaterSurfaceLocation.Z + BreachHeight,
			WaterSurfaceLocation.Z - SubmergeDepth, EasedAlpha);
	}
	else
	{
		Destroy();
		return;
	}

	SetActorLocation(NewLocation);
	if (bToppleStarted && ToppleDuration > 0.0f)
	{
		const float ToppleAlpha = FMath::Clamp((ElapsedSequenceTime - RiseEndTime) / ToppleDuration, 0.0f, 1.0f);
		const float EasedToppleAlpha = FMath::InterpEaseOut(0.0f, 1.0f, ToppleAlpha, 2.0f);
		SetActorRotation(FMath::Lerp(InitialActorRotation, ToppleTargetRotation, EasedToppleAlpha));
	}
	if (bToppleStarted && !bToppleImpactTriggered &&
		(ToppleDuration <= 0.0f || ElapsedSequenceTime >= RiseEndTime + ToppleDuration))
	{
		bToppleImpactTriggered = true;
		ApplyImpactToShip(WaterSurfaceLocation, ToppleImpactRadius);
	}
}

void AWhale::TriggerSurfaceImpact()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	ApplyImpactToShip(WaterSurfaceLocation, ShipImpactRadius);
}

void AWhale::TriggerToppleImpact()
{
	const FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(this, 0)
		? UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation()
		: WaterSurfaceLocation;
	FVector TowardPlayer = PlayerLocation - GetActorLocation();
	TowardPlayer.Z = 0.0f;
	TowardPlayer = TowardPlayer.GetSafeNormal();

	if (TowardPlayer.IsNearlyZero())
	{
		ToppleTargetRotation = InitialActorRotation;
	}
	else
	{
		const FVector LocalDirection = InitialActorRotation.UnrotateVector(TowardPlayer);
		// Unreal's positive pitch and roll tilt the actor's up axis away from these local directions.
		const float ForwardLean = -FMath::Clamp(LocalDirection.X, -1.0f, 1.0f) * ToppleAngle;
		const float SideLean = FMath::Clamp(LocalDirection.Y, -1.0f, 1.0f) * ToppleAngle;
		ToppleTargetRotation = FRotator(
			InitialActorRotation.Pitch + ForwardLean,
			InitialActorRotation.Yaw,
			InitialActorRotation.Roll + SideLean);
	}
}

void AWhale::ApplyImpactToShip(const FVector& VisualLocation, const float ImpactRadius)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (ImpactVisualClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FRotator VisualRotation(0.0f, InitialActorRotation.Yaw, 0.0f);
		World->SpawnActor<AAutoDestructible>(ImpactVisualClass, VisualLocation, VisualRotation, SpawnParameters);
	}

	ACargoCharacter* Ship = Cast<ACargoCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!IsValid(Ship))
	{
		return;
	}

	FVector ToShip = Ship->GetActorLocation() - WaterSurfaceLocation;
	ToShip.Z = 0.0f;
	const float SafeImpactRadius = FMath::Max(ImpactRadius, 0.0f);
	if (SafeImpactRadius <= 0.0f || ToShip.SizeSquared() > FMath::Square(SafeImpactRadius))
	{
		return;
	}

	const float Side = FVector::DotProduct(GetActorRightVector(), ToShip.GetSafeNormal());
	const float RollDirection = FMath::IsNearlyZero(Side) ? 1.0f : FMath::Sign(Side);
	Ship->ApplyExternalImpact(WaterSurfaceLocation, RollDirection * ShipImpactRollDegrees, ShipImpactPushStrength);
}
