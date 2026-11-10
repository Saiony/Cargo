// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid/Visuals/GridBasedPlaceableVisual.h"

#include "DataAssets/ContainerDA.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Components/StaticMeshComponent.h"

void AGridBasedPlaceableVisual::Initialize(TObjectPtr<UContainerDA> Data)
{
	Super::Initialize(Data);
	
	ClearMeshes();

	if (!Data)
	{
		return;
	}

	const auto Settings = GetDefault<UCargoSettings>();
	const auto SimpleCube = Settings->SimpleCube.Get();
	const auto GridSize = Settings->GridCellSize;

	for (const auto& Cell : Data->Shape.Cells)
	{
		UStaticMeshComponent* Cube = NewObject<UStaticMeshComponent>(this);

		Cube->SetStaticMesh(SimpleCube);
		Cube->SetMaterial(0, Data->Material);
		Cube->SetupAttachment(RootComponent);
		Cube->SetRelativeLocation(FVector(Cell) * GridSize);

		Cube->SetCollisionProfileName("PlaceablePhysics");

		Cube->RegisterComponent();
		GeneratedMeshes.Add(Cube);
	}
}

void AGridBasedPlaceableVisual::ClearMeshes()
{
	for (UStaticMeshComponent* Mesh : GeneratedMeshes)
	{
		if (IsValid(Mesh))
		{
			Mesh->DestroyComponent();
		}
	}

	GeneratedMeshes.Empty();
}
