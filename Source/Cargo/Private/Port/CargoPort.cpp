// Fill out your copyright notice in the Description page of Project Settings.

#include "Port/CargoPort.h"

#include "GameplayTagContainer.h"
#include "Components/StaticMeshComponent.h"
#include "Grid/Container.h"
#include "Grid/Placeable.h"
#include "CargoGameMode.h"

static TAutoConsoleVariable<bool> CVarCargoDebugDraw(TEXT("Cargo.DebugDraw"), false,
                                                     TEXT("Ativa o debug draw das áreas de spawn de containers (box + grid).\n")
                                                     TEXT("0: desativado, 1: ativado"),
                                                     ECVF_Cheat
);

UCargoPortComponent::UCargoPortComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCargoPortComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCargoPortComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (CVarCargoDebugDraw.GetValueOnGameThread())
	{
		DebugDrawSpawnGrid(0.1f);
	}	
}

FVector UCargoPortComponent::GetNextSpawnLocation()
{
	FVector Origin = GetComponentLocation();
	FVector BoxExtent = SpawnAreaExtent;
	FRotator Rotation = GetComponentRotation();

	// Quantos containers cabem por linha, baseado no espaçamento
	int32 ContainersPerRow = FMath::Max(1, FMath::FloorToInt((BoxExtent.X * 2.0f) / Spacing));

	// Posição local baseada nos índices atuais (grid)
	float LocalX = -BoxExtent.X + (CurrentColumn * Spacing);
	float LocalY = -BoxExtent.Y + (CurrentRow * Spacing);

	FVector LocalOffset(LocalX, LocalY, 0.0f);
	FVector NewLocation = Origin + Rotation.RotateVector(LocalOffset);

	// Avança para a próxima posição da grade
	CurrentColumn++;
	if (CurrentColumn >= ContainersPerRow)
	{
		CurrentColumn = 0;
		CurrentRow++;
	}

	return NewLocation;
}

void UCargoPortComponent::DebugDrawSpawnGrid(float Duration) const
{
#if ENABLE_DRAW_DEBUG
	FVector Origin = GetComponentLocation();
	FRotator Rotation = GetComponentRotation();
	FVector BoxExtent = SpawnAreaExtent;

	int32 ContainersPerRow = FMath::Max(1, FMath::FloorToInt((BoxExtent.X * 2.0f) / Spacing));
	int32 RowsPerArea = FMath::Max(1, FMath::FloorToInt((BoxExtent.Y * 2.0f) / Spacing));

	for (int32 Row = 0; Row < RowsPerArea; Row++)
	{
		for (int32 Col = 0; Col < ContainersPerRow; Col++)
		{
			float LocalX = -BoxExtent.X + (Col * Spacing);
			float LocalY = -BoxExtent.Y + (Row * Spacing);

			FVector LocalOffset(LocalX, LocalY, 0.0f);
			FVector WorldPos = Origin + Rotation.RotateVector(LocalOffset);

			DrawDebugSphere(GetWorld(), WorldPos, 25.0f, 8, FColor::Yellow, false, Duration);
		}
	}
#endif
}

void UCargoPortComponent::AddPlaceableToGrid(APlaceable* Placeable, FVector WorldPos)
{
	if (!IsOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Attempt to add placeable to a closed port"));
		return;
	}
	
	const FVector LocalPosition = GetOwner()->GetActorTransform().InverseTransformPosition(WorldPos);	
	PlaceableGrid.Add(LocalPosition.X, LocalPosition.Y, Placeable);	
	
	Placeable->SetActorLocation(WorldPos, false);
	
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
	Placeable->AttachToComponent(this, AttachmentRules);
	
	Placeable->Init(this, LocalPosition.X, LocalPosition.Y);
	
	//add to cargomap and register for quest
	if (auto Container = Cast<AContainer>(Placeable))
	{
		if (Container == nullptr)
			UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Placeable is not a container"));
		
		CargoMap.FindOrAdd(Container->ContainerDA->CargoTag)++;
		ACargoGameMode::Get(this)->RegisterCargoDelivery(CurrentQuestTag, Container->ContainerDA->CargoTag);		
	}
}

void UCargoPortComponent::OnPlaceableGrabbed_Implementation(APlaceable* Placeable)
{
	IGridActorInterface::OnPlaceableGrabbed_Implementation(Placeable);
	
	if (!IsOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Attempt to add placeable to a closed port"));
		return;
	}
	
	Placeable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);	
	PlaceableGrid.Remove(Placeable->GridPos.X, Placeable->GridPos.Y);		
	Placeable->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	
	if (auto Container = Cast<AContainer>(Placeable))
	{
		CargoMap[Container->ContainerDA->CargoTag]--;

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
	PlaceableGrid.Clear();
	
	for (auto Placeable : PlaceableGrid.GetValuesArray())
	{
		Placeable->Destroy();
	}	
}