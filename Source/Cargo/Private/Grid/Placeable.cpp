// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Grid/Placeable.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APlaceable::APlaceable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
	
	PivotComp = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	PivotComp->SetupAttachment(RootComponent);
	
	ContainerMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ContainerMesh"));
	ContainerMeshComp->SetupAttachment(PivotComp);
	
}

// Called when the game starts or when spawned
void APlaceable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlaceable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlaceable::Grab()
{	
	if (OwningGridActor == nullptr)
		return;
	
	OwningGridActor->RemovePlaceableFromGrid(this);
}

void APlaceable::Init(TObjectPtr<UGridComponent>GridActor, int32 GridPosX, int32 GridPosY)
{
	OwningGridActor = GridActor;
	PivotGridPos = FIntPoint(GridPosX, GridPosY);
}

void APlaceable::RotateClockwise()
{	
	const FRotator NewRotation = PivotComp->GetRelativeRotation() + FRotator(0, 90.0f, 0);
	PivotComp->SetRelativeRotation(NewRotation);
	LocalYaw = NewRotation.Yaw;
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

			if (FMath::IsNearlyEqual(Rotation, 0.0f)) // 0 degrees
			{
				RotatedX = OffsetX;
				RotatedY = OffsetY;
			}
			else if (FMath::IsNearlyEqual(Rotation, 90.0f)) // 90 degrees
			{
				RotatedX = -OffsetY;
				RotatedY = OffsetX;
			}
			else if (FMath::IsNearlyEqual(Rotation, 180.0f)) // 180 degrees
			{
				RotatedX = -OffsetX;
				RotatedY = -OffsetY;
			}
			else if (FMath::IsNearlyEqual(Rotation, 270.0f)) // 270 degrees
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

			Locations.Add(FVector(BaseLocation.X + RotatedX, BaseLocation.Y + RotatedY, 0.0f));
		}
	}

	return Locations;
}

