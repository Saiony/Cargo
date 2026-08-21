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
 
	BuoyancyComp->SetComponentTickEnabled(false);
    
	OwningGridActor = GridActor;
	PivotGridPos = FIntPoint(GridPosX, GridPosY);
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

    for (int i = 0; i < Size.X; i++)
    {
       for (int j = 0; j < Size.Y; j++)
       {
          const auto OffsetX = i * CellSize;
          const auto OffsetY = j * CellSize;

          float RotatedX, RotatedY;

          if (FMath::IsNearlyEqual(Rotation, 0.0f))
          {
             RotatedX = OffsetX;
             RotatedY = OffsetY;
          }
          else if (FMath::IsNearlyEqual(Rotation, 90.0f))
          {
             RotatedX = -OffsetY;
             RotatedY = OffsetX;
          }
          else if (FMath::IsNearlyEqual(Rotation, 180.0f))
          {
             RotatedX = -OffsetX;
             RotatedY = -OffsetY;
          }
          else if (FMath::IsNearlyEqual(Rotation, 270.0f))
          {
             RotatedX = OffsetY;
             RotatedY = -OffsetX;
          }
          else
          {
             UE_LOG(LogTemp, Error, TEXT("Invalid rotation angle: %f"), Rotation);
             
             RotatedX = OffsetX;
             RotatedY = OffsetY;
          }

          Locations.Add(FVector(BaseLocation.X + RotatedX, BaseLocation.Y + RotatedY, BaseLocation.Z));
       }
    }

    return Locations;
}

bool APlaceable::IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable)
{
    if (OwningGridActor == nullptr)
       return false;
    
    return OwningGridActor->IsPlaceableBlocked(Placeable);
}