// Copyright Epic Games, Inc. All Rights Reserved.

#include "Whale/WhaleSpawner.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Whale/Whale.h"

AWhaleSpawner::AWhaleSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	ShadowVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShadowVisual"));
	ShadowVisual->SetupAttachment(SceneRoot);
	ShadowVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShadowVisual->SetCastShadow(false);
	ShadowVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 2.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		ShadowVisual->SetStaticMesh(PlaneMesh.Object);
	}
	WhaleClass = AWhale::StaticClass();
}

void AWhaleSpawner::InitializeEncounter(const FVector& InWaterSurfaceLocation)
{
	WaterSurfaceLocation = InWaterSurfaceLocation;
	bHasWaterSurfaceLocation = true;
	SetActorLocation(WaterSurfaceLocation);
}

void AWhaleSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const float PlaneScale = FMath::Max(ShadowSize, 0.0f) / 100.0f;
	ShadowVisual->SetRelativeScale3D(FVector(PlaneScale, PlaneScale, 1.0f));
	ShadowVisual->SetMaterial(0, ShadowMaterial);
	ShadowVisual->SetVisibility(ShadowMaterial != nullptr);
}

void AWhaleSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!bHasWaterSurfaceLocation)
	{
		WaterSurfaceLocation = GetActorLocation();
	}

	SetActorLocation(WaterSurfaceLocation);
	if (ShadowMaterial)
	{
		ShadowVisual->SetMaterial(0, ShadowMaterial);
		ShadowVisual->SetVisibility(true);
	}

	if (ShadowLeadTime <= 0.0f)
	{
		SpawnWhale();
	}
	else if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(WhaleSpawnTimer, this, &AWhaleSpawner::SpawnWhale, ShadowLeadTime, false);
	}
}

void AWhaleSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WhaleSpawnTimer);
	}

	if (AWhale* Whale = SpawnedWhale.Get(); IsValid(Whale) && !Whale->IsActorBeingDestroyed())
	{
		Whale->OnDestroyed.RemoveDynamic(this, &AWhaleSpawner::OnWhaleDestroyed);
		Whale->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void AWhaleSpawner::SpawnWhale()
{
	if (!WhaleClass || !GetWorld())
	{
		Destroy();
		return;
	}

	const FVector SpawnLocation = WaterSurfaceLocation - FVector(0.0f, 0.0f, WhaleSpawnDepth);
	const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
	AWhale* Whale = GetWorld()->SpawnActorDeferred<AWhale>(
		WhaleClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!IsValid(Whale))
	{
		Destroy();
		return;
	}

	Whale->InitializeBreach(WaterSurfaceLocation, WhaleSpawnDepth);
	Whale->OnDestroyed.AddDynamic(this, &AWhaleSpawner::OnWhaleDestroyed);
	SpawnedWhale = Whale;
	UGameplayStatics::FinishSpawningActor(Whale, SpawnTransform);
}

void AWhaleSpawner::OnWhaleDestroyed(AActor* DestroyedActor)
{
	SpawnedWhale.Reset();
	Destroy();
}
