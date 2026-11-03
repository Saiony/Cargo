// Copyright Epic Games, Inc. All Rights Reserved.

#include "TurtleEnemySpawner.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "TurtleEnemy.h"

ATurtleEnemySpawner::ATurtleEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	MovementSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MovementSpline"));
	MovementSpline->SetupAttachment(RootComponent);
}

void ATurtleEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnAtBeginPlay)
	{
		SpawnTurtle();
		GetWorld()->GetTimerManager().SetTimer(
			SpawnTimer,
			this,
			&ATurtleEnemySpawner::SpawnTurtleOnTimer,
			FMath::Max(SpawnInterval, 0.1f),
			true);
	}
}

void ATurtleEnemySpawner::SpawnTurtleOnTimer()
{
	SpawnTurtle();
}

void ATurtleEnemySpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimer);
	}

	Super::EndPlay(EndPlayReason);
}

ATurtleEnemy* ATurtleEnemySpawner::SpawnTurtle()
{
	if (!IsValid(TurtleClass) || !IsValid(MovementSpline) || MovementSpline->GetSplineLength() <= KINDA_SMALL_NUMBER)
	{
		return nullptr;
	}

	const FVector StartLocation = MovementSpline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
	const FVector StartDirection = MovementSpline->GetDirectionAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
	const FTransform SpawnTransform(StartDirection.Rotation(), StartLocation);

	ATurtleEnemy* Turtle = GetWorld()->SpawnActorDeferred<ATurtleEnemy>(
		TurtleClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!IsValid(Turtle))
	{
		return nullptr;
	}

	Turtle->SetSplineToFollow(MovementSpline);
	UGameplayStatics::FinishSpawningActor(Turtle, SpawnTransform);
	return Turtle;
}
