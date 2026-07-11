// Fill out your copyright notice in the Description page of Project Settings.

#include "Port/CargoPort.h"

#include "GameplayTagContainer.h"
#include "Components/StaticMeshComponent.h"
#include "Grid/Container.h"
#include "Grid/Placeable.h"
#include "CargoGameMode.h"

UCargoPortComponent::UCargoPortComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCargoPortComponent::BeginPlay()
{
	Super::BeginPlay();

	OnPlaceableAddedToGrid.AddDynamic(this, &UCargoPortComponent::HandlePlaceableAddedToGrid);
	OnPlaceableRemovedFromGrid.AddDynamic(this, &UCargoPortComponent::HandlePlaceableRemovedFromGrid);
}

void UCargoPortComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
#if !UE_BUILD_SHIPPING
		DrawDebugGrid(0.1f);
#endif
}

FVector UCargoPortComponent::GetNextSpawnLocation()
{
	const int32 CellSize = PlaceableGrid.GetCellSize();
	const FIntPoint Min = PlaceableGrid.GetMin();
	const FIntPoint Max = PlaceableGrid.GetMax();

	const int32 ContainersPerRow = FMath::Max(1, Max.X - Min.X + 1);

	const FIntPoint GridIndex(Min.X + CurrentColumn, Min.Y + CurrentRow);
	const FVector LocalPos = PlaceableGrid.GridToLocal(GridIndex);
	const FVector WorldPos = GetComponentTransform().TransformPosition(LocalPos);

	CurrentColumn++;
	if (CurrentColumn >= ContainersPerRow)
	{
		CurrentColumn = 0;
		CurrentRow++;
	}

	return WorldPos;
}

void UCargoPortComponent::AttachPlaceable(APlaceable* Placeable, FVector WorldPos)
{    
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	Placeable->AttachToComponent(this, AttachmentRules);
}

void UCargoPortComponent::AddPlaceable(APlaceable* Placeable, FVector WorldPos)
{
	if (!IsOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Attempt to add placeable to a closed port"));
		return;
	}

	AddPlaceableToGrid(Placeable, WorldPos, 0.0f);

	Placeable->SetActorLocation(WorldPos, false);
	AttachPlaceable(Placeable, WorldPos);
	Placeable->SetActorRotation(FRotator(0, 0, 0));
}

void UCargoPortComponent::HandlePlaceableAddedToGrid(APlaceable* Placeable)
{
	if (auto Container = Cast<AContainer>(Placeable))
	{
		if (ACargoGameMode* GM = ACargoGameMode::Get(this))
		{
			GM->RegisterCargoDelivery(CurrentQuestTag, Container->ContainerDA->CargoTag);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Placeable is not a container"));
	}
}

void UCargoPortComponent::HandlePlaceableRemovedFromGrid(APlaceable* Placeable)
{
	if (!IsOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Placeable removed while port is closed"));
	}

	Placeable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (auto Container = Cast<AContainer>(Placeable))
	{
		if (ACargoGameMode* GM = ACargoGameMode::Get(this))
		{
			GM->RemoveCargoDelivery(CurrentQuestTag, Container->ContainerDA->CargoTag);
		}
	}
}

void UCargoPortComponent::StartQuestDelivery(FGameplayTag QuestTag)
{
	IsOpen = true;
	CurrentQuestTag = QuestTag;	
}

void UCargoPortComponent::Clear()
{
	IsOpen = false;
	CurrentQuestTag = FGameplayTag();

	ClearGrid();
}