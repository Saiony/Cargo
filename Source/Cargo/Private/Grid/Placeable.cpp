// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Grid/Placeable.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Grid/PlaceablePreview.h"

void APlaceable::UpdateDropHover(APlaceable* Placeable, const FVector& ImpactPoint, APlaceablePreview* Preview)
{
    // Stacking on a placeable uses the same placement rules as its grid.
    if (IsValid(OwningGridActor))
        OwningGridActor->UpdateDropHover(Placeable, ImpactPoint, Preview);
    else
        Preview->SetActorHiddenInGame(true);
}

bool APlaceable::TryAcceptDrop(APlaceable* Placeable, APlaceablePreview* Preview)
{
    if (IsValid(OwningGridActor))
        return OwningGridActor->TryAcceptDrop(Placeable, Preview);

    Placeable->Release();
    return true;
}

APlaceable::APlaceable()
{
    PrimaryActorTick.bCanEverTick = true;

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    SetRootComponent(BoxComp);
    BoxComp->SetCollisionProfileName(TEXT("PhysicsActor"));
    BoxComp->SetSimulatePhysics(false);

    StackLeanPivot = CreateDefaultSubobject<USceneComponent>(TEXT("StackLeanPivot"));
    StackLeanPivot->SetupAttachment(RootComponent);

    PivotComp = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComp"));
    PivotComp->SetupAttachment(StackLeanPivot);

    BuoyancyComp = CreateDefaultSubobject<UBuoyancyComponent>(TEXT("BuoyancyComp"));	
	
	PlaceableVisualComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("PlaceableVisualComp"));
	PlaceableVisualComp->SetupAttachment(PivotComp);
}

void APlaceable::BeginPlay()
{
    Super::BeginPlay();

    // Existing Blueprints may retain the old attachment directly to the root.
    if (PivotComp->GetAttachParent() != StackLeanPivot)
        PivotComp->AttachToComponent(StackLeanPivot, FAttachmentTransformRules::KeepRelativeTransform);
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

    OwningGridActor->RemovePlaceableFromGrid(this);
    SetActorRotation(FRotator(0.f, LocalYaw, 0.f));
    
    UGameplayStatics::PlaySoundAtLocation(this, GrabSound, GetActorLocation());
}

void APlaceable::Place(TObjectPtr<UGridComponent> GridActor, int32 GridPosX, int32 GridPosY, int32 GridPosZ)
{
	StackLeanPivot->SetRelativeTransform(FTransform::Identity);
	BoxComp->SetSimulatePhysics(false);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
 
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

int32 APlaceable::GetPlaceableRotation()
{
	return PivotComp->GetRelativeRotation().Yaw;
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

TArray<FIntVector> APlaceable::GetAllGridPositionsIndex(float Rotation) const
{   
	TArray<FIntVector> Locations;
	
	for (const auto Cell : GridShapeDefinition.GetRotatedCells(Rotation))
	{		
		Locations.Add(Cell);
	}

	return Locations;
}

TArray<FIntVector> APlaceable::GetAllGridPositionsIndex(FIntVector GridPos, float Rotation)
{   
	auto LocalGridPos = GetAllGridPositionsIndex(Rotation);
	
	for (auto& Cell : LocalGridPos)
	{		
		Cell += GridPos;
	}
	
	return LocalGridPos;
}

bool APlaceable::IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable)
{
    if (OwningGridActor == nullptr)
       return false;
    
    return OwningGridActor->IsPlaceableBlocked(Placeable);
}

void APlaceable::FallIntoSea(const FVector& Direction, const FVector& InheritedVelocity)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetSimulatePhysics(true);

	BoxComp->SetPhysicsLinearVelocity(InheritedVelocity, false, NAME_None);
	BoxComp->AddImpulse(Direction.GetSafeNormal() * 150.0f, NAME_None, true);
	BoxComp->AddAngularImpulseInDegrees(
		FMath::VRand() * 35.0f,
		NAME_None,
		true
	);
	
	OwningGridActor = nullptr;
}

void APlaceable::UpdateMesh()
{
	// InstanceMeshComp->ClearInstances();
	// InstanceMeshComp->SetMaterial(0, PlaceableDA->Material);
	//
	// const auto GridCellSize = GetDefault<UCargoSettings>()->GridCellSize;
	//
	// for (const FIntVector& Cell : PlaceableDA->Shape.Cells)
	// {
	// 	const FVector LocalLocation = FVector(
	// 		Cell.X * GridCellSize,
	// 		Cell.Y * GridCellSize,
	// 		Cell.Z * GridCellSize
	// 	);
	//
	// 	InstanceMeshComp->AddInstance(FTransform(LocalLocation));
	// }
}

void APlaceable::SetStackLean(const FTransform& GridDeformation, const FTransform& GridWorldTransform)
{
	const FVector GridPosition = GridWorldTransform.InverseTransformPosition(GetActorLocation());
	const FVector Location = GridWorldTransform.TransformPosition(GridDeformation.TransformPosition(GridPosition));
	const FQuat Rotation = GridWorldTransform.GetRotation() * GridDeformation.GetRotation() *
		GridWorldTransform.GetRotation().Inverse() * GetActorQuat();
	StackLeanPivot->SetWorldLocationAndRotation(Location, Rotation);
}

void APlaceable::BakeStackLean()
{
	// Once cargo leaves the grid, its physics root takes over the visible pose.
	const FTransform VisualPose = StackLeanPivot->GetComponentTransform();
	StackLeanPivot->SetRelativeTransform(FTransform::Identity);
	SetActorTransform(VisualPose, false, nullptr, ETeleportType::TeleportPhysics);
}
