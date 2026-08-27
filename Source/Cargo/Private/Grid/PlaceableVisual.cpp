// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/PlaceableVisual.h"

#include "DeveloperSettings/CargoSettings.h"


// Sets default values for this component's properties
APlaceableVisual::APlaceableVisual()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	// ...
}

void APlaceableVisual::Initialize(TObjectPtr<UContainerDA> Data)
{
	ClearMeshes();
	
	const auto Settings = GetDefault<UCargoSettings>();
	const auto SimpleCube = Settings->SimpleCube.Get();
	const auto GridSize = Settings->GridCellSize;
	
	for (const auto& Cell : Data->Shape.Cells)
	{
		const auto Cube = NewObject<UStaticMeshComponent>(this);

		Cube->SetStaticMesh(SimpleCube);
		Cube->SetMaterial(0, Data->Material);
		Cube->SetupAttachment(GetRootComponent());
		Cube->SetRelativeLocation(FVector(Cell) * GridSize);
		
		Cube->SetCollisionProfileName("PlaceableNoPhysics");
		
		Cube->RegisterComponent();
	}	
}

void APlaceableVisual::ClearMeshes()
{
	TArray<USceneComponent*> CubeChildren;
	RootComponent->GetChildrenComponents(true, CubeChildren);
	for (auto& Child : CubeChildren)
	{
		Child->DestroyComponent();
	}
}


// Called when the game starts
void APlaceableVisual::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}