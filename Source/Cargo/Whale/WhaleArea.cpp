// Copyright Epic Games, Inc. All Rights Reserved.

#include "Whale/WhaleArea.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "WaterBodyActor.h"
#include "Whale/WhaleSpawner.h"

AWhaleArea::AWhaleArea()
{
	PrimaryActorTick.bCanEverTick = false;

	DetectionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionVolume"));
	SetRootComponent(DetectionVolume);
	DetectionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionVolume->SetCollisionObjectType(ECC_WorldDynamic);
	DetectionVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionVolume->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	DetectionVolume->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	DetectionVolume->SetGenerateOverlapEvents(true);
	DetectionVolume->OnComponentBeginOverlap.AddDynamic(this, &AWhaleArea::OnDetectionBeginOverlap);
	DetectionVolume->OnComponentEndOverlap.AddDynamic(this, &AWhaleArea::OnDetectionEndOverlap);
	WhaleSpawnerClass = AWhaleSpawner::StaticClass();
}

void AWhaleArea::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DetectionVolume->SetBoxExtent(DetectionBoxExtent);
}

void AWhaleArea::BeginPlay()
{
	Super::BeginPlay();

	// Also handle a player who is already inside the volume when play starts.
	TArray<AActor*> OverlappingActors;
	DetectionVolume->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
	for (AActor* Actor : OverlappingActors)
	{
		if (APawn* Pawn = Cast<APawn>(Actor); IsValid(Pawn) && Pawn->IsPlayerControlled())
		{
			StartSpawnTimer(Pawn);
			break;
		}
	}
}

void AWhaleArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopSpawnTimer();
	Super::EndPlay(EndPlayReason);
}

void AWhaleArea::OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (IsValid(Pawn) && Pawn->IsPlayerControlled())
	{
		StartSpawnTimer(Pawn);
	}
}

void AWhaleArea::OnDetectionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	// A pawn can overlap with several components; only stop after its last overlap ends.
	if (OtherActor == ActivePlayerPawn.Get() && !DetectionVolume->IsOverlappingActor(OtherActor))
	{
		StopSpawnTimer();
	}
}

void AWhaleArea::StartSpawnTimer(APawn* PlayerPawn)
{
	if (!IsValid(PlayerPawn) || ActivePlayerPawn.Get() == PlayerPawn || !GetWorld())
	{
		return;
	}

	ActivePlayerPawn = PlayerPawn;
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer,
		this,
		&AWhaleArea::SpawnWhaleSpawner,
		FMath::Max(SpawnInterval, 0.1f),
		true);
}

void AWhaleArea::StopSpawnTimer()
{
	ActivePlayerPawn.Reset();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimer);
	}
}

bool AWhaleArea::FindWaterSurface(const FVector& NearLocation, FVector& OutSurfaceLocation) const
{
	const auto TryWaterBody = [&NearLocation, &OutSurfaceLocation](AWaterBody* Candidate) -> bool
	{
		if (!IsValid(Candidate) || !IsValid(Candidate->GetWaterBodyComponent()))
		{
			return false;
		}

		FVector SurfaceNormal = FVector::UpVector;
		FVector WaterVelocity = FVector::ZeroVector;
		float WaterDepth = 0.0f;
		return Candidate->GetWaterBodyComponent()->GetWaterSurfaceInfoAtLocation(
			NearLocation, OutSurfaceLocation, SurfaceNormal, WaterVelocity, WaterDepth, false);
	};

	if (TryWaterBody(WaterBody))
	{
		return true;
	}

	for (TActorIterator<AWaterBody> It(GetWorld()); It; ++It)
	{
		if (TryWaterBody(*It))
		{
			return true;
		}
	}

	OutSurfaceLocation = FVector(NearLocation.X, NearLocation.Y, FallbackWaterSurfaceZ);
	return false;
}

void AWhaleArea::SpawnWhaleSpawner()
{
	APawn* PlayerPawn = ActivePlayerPawn.Get();
	if (!IsValid(PlayerPawn))
	{
		StopSpawnTimer();
		return;
	}

	if (!WhaleSpawnerClass || !GetWorld())
	{
		return;
	}

	const float MinRadius = FMath::Max(MinSpawnRadius, 0.0f);
	const float MaxRadius = FMath::Max(MaxSpawnRadius, MinRadius);
	const float Distance = FMath::Sqrt(FMath::Lerp(
		FMath::Square(MinRadius), FMath::Square(MaxRadius), FMath::FRand()));
	const FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector Forward = PlayerPawn->GetActorForwardVector();
	Forward.Z = 0.0f;
	Forward = Forward.GetSafeNormal();
	if (Forward.IsNearlyZero())
	{
		Forward = FVector::ForwardVector;
	}
	const FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
	const float SafeFrontSpawnHalfAngle = FMath::Clamp(FrontSpawnHalfAngle, 0.0f, 90.0f);
	const float FrontArcAngle = FMath::DegreesToRadians(
		FMath::FRandRange(-SafeFrontSpawnHalfAngle, SafeFrontSpawnHalfAngle));
	const FVector SpawnDirection = Forward * FMath::Cos(FrontArcAngle) + Right * FMath::Sin(FrontArcAngle);
	const FVector CandidateLocation = PlayerLocation + SpawnDirection * Distance;

	FVector SurfaceLocation;
	FindWaterSurface(CandidateLocation, SurfaceLocation);

	const FTransform SpawnTransform(FRotator::ZeroRotator, SurfaceLocation);
	AWhaleSpawner* WhaleSpawner = GetWorld()->SpawnActorDeferred<AWhaleSpawner>(
		WhaleSpawnerClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!IsValid(WhaleSpawner))
	{
		return;
	}

	WhaleSpawner->InitializeEncounter(SurfaceLocation);
	UGameplayStatics::FinishSpawningActor(WhaleSpawner, SpawnTransform);
}
