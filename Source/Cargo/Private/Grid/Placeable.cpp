// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Grid/Placeable.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

APlaceable::APlaceable()
{
    PrimaryActorTick.bCanEverTick = true;

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    SetRootComponent(BoxComp);
    BoxComp->SetCollisionProfileName(TEXT("PhysicsActor"));
    BoxComp->SetSimulatePhysics(false);

    PivotComp = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComp"));
    PivotComp->SetupAttachment(RootComponent);

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(PivotComp);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetSimulatePhysics(false);

    BuoyancyComp = CreateDefaultSubobject<UBuoyancyComponent>(TEXT("BuoyancyComp"));
}

void APlaceable::BeginPlay()
{
    Super::BeginPlay();
}

void APlaceable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APlaceable::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APlaceable::Grab()
{   
    if (OwningGridActor == nullptr)
       return;
    
    BoxComp->SetSimulatePhysics(false);
    BoxComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    BoxComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
    
    BuoyancyComp->SetComponentTickEnabled(false);

    SetActorRotation(FRotator(0.f, LocalYaw, 0.f));
    
    OwningGridActor->RemovePlaceableFromGrid(this);
    
    UGameplayStatics::PlaySoundAtLocation(this, GrabSound, GetActorLocation());
}

void APlaceable::Place(TObjectPtr<UGridComponent> GridActor, int32 GridPosX, int32 GridPosY, int32 GridPosZ)
{
	BoxComp->SetSimulatePhysics(false);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
 
	BuoyancyComp->SetComponentTickEnabled(false);
    
	OwningGridActor = GridActor;
	PivotGridPos = FIntVector(GridPosX, GridPosY, GridPosZ);
	this->GridLevel = GridLevel;
 
	UGameplayStatics::PlaySoundAtLocation(this, PlaceSound, GetActorLocation());
}

void APlaceable::Release()
{
    BoxComp->SetSimulatePhysics(true);	
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    BuoyancyComp->SetComponentTickEnabled(true);
    //BuoyancyComp->Activate();
    
    OwningGridActor = nullptr;
}

void APlaceable::RotateClockwise()
{
    LocalYaw = FRotator::NormalizeAxis(LocalYaw + 90.0f);

    FRotator Rotation = PivotComp->GetRelativeRotation();
    Rotation.Yaw = LocalYaw;

    PivotComp->SetRelativeRotation(Rotation);
}

void APlaceable::AlignToRotation(const FRotator& ReferenceRotation)
{
    SetActorRotation(FRotator(0.f, ReferenceRotation.Yaw, 0.f));
}

TArray<FVector> APlaceable::GetAllGridPositions(const FVector& BaseLocation, float Rotation, float CellSize) const
{   
	TArray<FVector> Locations;

	for (const auto Cell : GridShapeDefinition.GetRotatedCells(Rotation))
	{
		FVector LocalCellPos = FVector(Cell.X * CellSize, 
									   Cell.Y * CellSize, 
									   Cell.Z * CellSize);
		
		FVector WorldCellPos = BaseLocation + LocalCellPos;
		Locations.Add(WorldCellPos);
	}

	return Locations;
}

TArray<FIntVector> APlaceable::GetAllGridPositionsIndex(const FVector& BaseLocation, float Rotation) const
{   
	TArray<FIntVector> Locations;
	
	for (const auto Cell : GridShapeDefinition.GetRotatedCells(Rotation))
	{		
		Locations.Add(Cell);
	}

	return Locations;
}

bool APlaceable::IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable)
{
    if (OwningGridActor == nullptr)
       return false;
    
    return OwningGridActor->IsPlaceableBlocked(Placeable);
}

void APlaceable::FallIntoSea(const FVector& Direction)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetSimulatePhysics(true);

	const FVector UpImpulse = FVector::UpVector * 1000.0f;
	const FVector SideImpulse = -Direction.GetSafeNormal() * 5000.0f;

	BoxComp->AddImpulse(UpImpulse + SideImpulse, NAME_None, true);
	BoxComp->AddAngularImpulseInDegrees(
		FMath::VRand() * 500.0f,
		NAME_None,
		true
	);
}