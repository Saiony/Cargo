// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridComponent.h"

#include "Chaos/PBDRigidsEvolution.h"

UGridComponent::UGridComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGridComponent::BeginPlay()
{
	Super::BeginPlay();	
	InitializeGrid(GetDefault<UCargoSettings>()->GridCellSize, FIntPoint(0, 0), GridSize);
}

void UGridComponent::OnPlaceableAdded(APlaceable* Placeable)
{
	IGridActorInterface::OnPlaceableAdded(Placeable);
}

void UGridComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	
	DrawDebugGrid(0.f);
}

void UGridComponent::ClearGrid()
{
	for (auto Placeable : GetOccupiedSlots())
	{
		Placeable.Value->Destroy();
	}
	
	PlaceableGrid.Clear();
}

