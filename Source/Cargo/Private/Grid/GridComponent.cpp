// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridComponent.h"

#include "ConsoleVariables.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Grid/Placeable.h"
#include "Grid/Container.h"
#include "Grid/PlaceablePreview.h"

void UGridComponent::UpdateDropHover(APlaceable* Placeable, const FVector& ImpactPoint, APlaceablePreview* Preview)
{
	const FIntVector GridPosition = GetNextFreeZPositionGrid(ImpactPoint);
	if (CanAddPlaceableToGridIndex(Placeable, GridPosition, Placeable->GetLocalYaw()))
		Preview->SetValid();
	else
		Preview->SetInvalid();

	Preview->SetActorHiddenInGame(false);
	Preview->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	Preview->SetActorRelativeLocation(GridToLocalPos(GridPosition));
	Preview->SetActorRelativeRotation(FRotator::ZeroRotator);
	Preview->MimicPlaceableYaw(Placeable);
}

bool UGridComponent::TryAcceptDrop(APlaceable* Placeable, APlaceablePreview* Preview)
{
	if (!CanAddPlaceableToGrid(Placeable, Preview->GetActorLocation(), Placeable->GetLocalYaw()))
		return false;

	AddPlaceableToGrid(Placeable, Preview->GetActorLocation(), Placeable->GetLocalYaw());
	return true;
}

UGridComponent::UGridComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGridComponent::BeginPlay()
{
	Super::BeginPlay();    
	
	InitializeGrid();
}

void UGridComponent::OnPlaceableAdded(APlaceable* Placeable)
{
}

void UGridComponent::InitializeGrid()
{
	const auto GridCellSize = GetDefault<UCargoSettings>()->GridCellSize;
	const auto Origin = FIntVector(0, 0, 0);
	const auto GridSize = GridComponentDA->GridSize;	
	const auto InvalidSlots = GridComponentDA->InvalidSlots.Cells;	
	
	PlaceableGrid = UFROGGrid<APlaceable*>(GridCellSize, Origin, GridSize, InvalidSlots);		
	InstancedMeshComp->SetStaticMesh(GridComponentDA->CellMesh);
	
	for (const auto& Cell : PlaceableGrid.GetAllAvailableSlots())
	{		
		const auto LocalPos = GridToLocalPos(Cell);
		InstancedMeshComp->AddInstance(FTransform(LocalPos));
	}
}

FVector UGridComponent::WorldToLocal(const FVector& WorldLocation)
{
	return GetComponentTransform().InverseTransformPosition(WorldLocation);
}

void UGridComponent::OnRegister()
{
	Super::OnRegister();
	
	if (!InstancedMeshComp)
	{
		InstancedMeshComp = NewObject<UInstancedStaticMeshComponent>(GetOwner(),TEXT("InstancedMeshComp"));
		InstancedMeshComp->SetupAttachment(this);
		InstancedMeshComp->SetRelativeLocation(FVector(0, 0, -50));
		InstancedMeshComp->RegisterComponent();
		
		InstancedMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InstancedMeshComp->SetGenerateOverlapEvents(false);
		InstancedMeshComp->SetSimulatePhysics(false);
	}
	
}

bool UGridComponent::CanAddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector WorldLocation, float Rotation)
{
	if(!Placeable)
		return false;
    		
	const auto LocalLocation = WorldToLocal(WorldLocation);

	const FVector RoundedLocation = PlaceableGrid.GetRoundedLocation(LocalLocation);
	const TArray<FVector> OccupiedGridPositions = Placeable->GetAllGridPositions(RoundedLocation, Rotation, PlaceableGrid.GetCellSize());		

	for (const FVector& Pos : OccupiedGridPositions)
	{
		const auto GridIndex = PlaceableGrid.LocalToGrid(Pos);
    
		if (!PlaceableGrid.IsSlotAvailable(GridIndex))
			return false;
	}

	return true;
}

bool UGridComponent::CanAddPlaceableToGridIndex(TObjectPtr<APlaceable> Placeable, const FIntVector PlaceablePivotGridIndex, float Rotation)
{
	if(!Placeable)
		return false;
	
	const auto OccupiedGridIndex = Placeable->GetAllGridPositionsIndex(PlaceablePivotGridIndex, Rotation);		

	for (const auto GridIndex : OccupiedGridIndex)
	{    
		if (!PlaceableGrid.IsSlotAvailable(GridIndex))
			return false;
	}

	return true;
}


void UGridComponent::AddPlaceableToGrid(TObjectPtr<APlaceable> Placeable, const FVector& WorldLocation, float Rotation)
{
	if(!Placeable)
		return;
		
	if (!CanAddPlaceableToGrid(Placeable, WorldLocation, Rotation))
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to place Placeable on Grid"));
		return;
	}
		
	const auto LocalLocation = WorldToLocal(WorldLocation);
	const TArray<FVector> OccupiedLocations = Placeable->GetAllGridPositions(LocalLocation, Rotation, PlaceableGrid.GetCellSize());

	for (const FVector& Pos : OccupiedLocations)
	{
		const auto GridIndex = PlaceableGrid.LocalToGrid(Pos);
		PlaceableGrid.Add(GridIndex, Placeable);

		UE_LOG(LogTemp, Log, TEXT("Placeable added to grid [%d, %d] at world pos [%f, %f]"), GridIndex.X, GridIndex.Y, Pos.X, Pos.Y);
	}

	Placeable->Place(this, LocalLocation.X, LocalLocation.Y, LocalLocation.Z);	
	
	Placeable->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	// inherit location but zeroes the local rotation
	Placeable->SetActorRelativeLocation(LocalLocation);
	Placeable->SetActorRelativeRotation(FRotator(0.f, 0.f, 0.f));
	UpdateStackLean(CurrentStackRoll);

	OnPlaceableAddedToGrid.Broadcast(Placeable);
	OnPlaceableAdded(Placeable);
}

void UGridComponent::AddPlaceableToGridIndex(TObjectPtr<APlaceable> Placeable, const FIntVector PlaceablePivotGridIndex, float Rotation)
{
	if(!Placeable)
		return;
		
	if (!CanAddPlaceableToGridIndex(Placeable, PlaceablePivotGridIndex, Rotation))
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to place Placeable on Grid"));
		return;
	}
		
	const auto PlaceablePositions = Placeable->GetAllGridPositionsIndex(PlaceablePivotGridIndex, Rotation);

	for (const auto PlaceablePos : PlaceablePositions)
	{
		PlaceableGrid.Add(PlaceablePos, Placeable);

		UE_LOG(LogTemp, Log, TEXT("Placeable added to grid [%d, %d]"), PlaceablePos.X, PlaceablePos.Y);
	}

	Placeable->Place(this, PlaceablePivotGridIndex.X, PlaceablePivotGridIndex.Y, PlaceablePivotGridIndex.Z);	
	
	Placeable->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	// inherit location but zeroes the local rotation
	const auto LocalLocation = GridToLocalPos(PlaceablePivotGridIndex);
	Placeable->SetActorRelativeLocation(LocalLocation);
	Placeable->SetActorRelativeRotation(FRotator(0.f, 0.f, 0.f));
	UpdateStackLean(CurrentStackRoll);

	OnPlaceableAddedToGrid.Broadcast(Placeable);
	OnPlaceableAdded(Placeable);
}

void UGridComponent::RemovePlaceableFromGrid(TObjectPtr<APlaceable> Placeable)
{
	if(!Placeable)
		return;
		
	const auto LocalLocation = WorldToLocal(Placeable->GetActorLocation());

	const TArray<FVector> OccupiedPos = Placeable->GetAllGridPositions(LocalLocation, Placeable->GetLocalYaw(), PlaceableGrid.GetCellSize());

	for (const FVector& Pos : OccupiedPos)
	{
		const auto GridIndex = PlaceableGrid.LocalToGrid(Pos);
		PlaceableGrid.Remove(GridIndex.X, GridIndex.Y, GridIndex.Z);
		UE_LOG(LogTemp, Log, TEXT("Placeable removed from grid [%d, %d] at local pos [%f, %f]"), GridIndex.X, GridIndex.Y, Pos.X, Pos.Y);
	}
	
	Placeable->BakeStackLean();
	Placeable->OwningGridActor = nullptr;
	UpdateStackLean(CurrentStackRoll);
	OnPlaceableRemovedFromGrid.Broadcast(Placeable);
}

void UGridComponent::UpdateStackLean(float ShipRoll)
{
	CurrentStackRoll = ShipRoll;
	const auto Slots = GetOccupiedSlots();
	const float CellSize = GetCellSize();

	struct FStackEntry
	{
		APlaceable* Placeable = nullptr;
		TArray<FIntVector> Cells;
		int32 BottomZ = MAX_int32;
	};
	TArray<FStackEntry> Stack;
	TMap<APlaceable*, int32> EntryIndices;
	for (const auto& Slot : Slots)
	{
		if (!IsValid(Slot.Value))
			continue;
		int32* Index = EntryIndices.Find(Slot.Value);
		if (!Index)
		{
			const int32 NewIndex = Stack.AddDefaulted();
			Stack[NewIndex].Placeable = Slot.Value;
			Index = &EntryIndices.Add(Slot.Value, NewIndex);
		}
		FStackEntry& Entry = Stack[*Index];
		Entry.Cells.Add(Slot.Key);
		Entry.BottomZ = FMath::Min(Entry.BottomZ, Slot.Key.Z);
	}
	Stack.Sort([](const FStackEntry& A, const FStackEntry& B)
	{
		return A.BottomZ != B.BottomZ ? A.BottomZ < B.BottomZ :
			A.Placeable->GetUniqueID() < B.Placeable->GetUniqueID();
	});

	TMap<APlaceable*, FTransform> Deformations;
	for (const FStackEntry& Entry : Stack)
	{
		// Each rigid container follows the support with the largest contact area.
		TMap<APlaceable*, int32> Contacts;
		APlaceable* Support = nullptr;
		int32 BestContactCount = 0;
		for (const FIntVector& Cell : Entry.Cells)
		{
			if (Cell.Z != Entry.BottomZ)
				continue;
			APlaceable* Below = Slots.FindRef(Cell - FIntVector(0, 0, 1));
			if (Below && Deformations.Contains(Below))
			{
				const int32 Count = ++Contacts.FindOrAdd(Below);
				if (Count > BestContactCount || (Count == BestContactCount &&
					Support && Below->GetUniqueID() < Support->GetUniqueID()))
				{
					Support = Below;
					BestContactCount = Count;
				}
			}
		}

		int32 MinY = MAX_int32;
		int32 MaxY = MIN_int32;
		for (const FIntVector& Cell : Entry.Cells)
		{
			if (Cell.Z == Entry.BottomZ && (!Support ||
				Slots.FindRef(Cell - FIntVector(0, 0, 1)) == Support))
			{
				MinY = FMath::Min(MinY, Cell.Y);
				MaxY = FMath::Max(MaxY, Cell.Y);
			}
		}

		const float Roll = FMath::Clamp(ShipRoll * StackLeanMultiplier *
			(Entry.BottomZ - GetMin().Z + 1), -45.f, 45.f);
		// Hinge around the downhill edge of the supporting surface, not the box center.
		const FVector Hinge(0.f, (Roll >= 0.f ? MaxY + 0.5f : MinY - 0.5f) * CellSize,
			(Entry.BottomZ - 0.5f) * CellSize);
		const FVector SupportedHinge = Support ? Deformations.FindChecked(Support).TransformPosition(Hinge) : Hinge;
		const FQuat Rotation = FRotator(0.f, 0.f, Roll).Quaternion();
		const FTransform Deformation(Rotation, SupportedHinge - Rotation.RotateVector(Hinge));
		Deformations.Add(Entry.Placeable, Deformation);
		Entry.Placeable->SetStackLean(Deformation, GetComponentTransform());
	}
}

APlaceable* UGridComponent::GetPlaceableAt(const FIntVector GridPos)
{
	const auto PlaceablePtr = PlaceableGrid.GetValue(GridPos.X, GridPos.Y, GridPos.Z);
	
	if (!PlaceablePtr)
		return nullptr;
	
	return *PlaceablePtr;
}

TMap<FIntVector, APlaceable*> UGridComponent::GetOccupiedSlots() const
{
	return PlaceableGrid.GetOccupiedSlots();
}

FVector UGridComponent::GetNextFreeZPositionWorld(const FVector& WorldLocation)
{
	const FVector LocalLocation = WorldToLocal(WorldLocation);
	const FVector RoundedLocation = PlaceableGrid.GetRoundedLocation(LocalLocation);

	FIntVector GridIndex = PlaceableGrid.LocalToGrid(RoundedLocation);

	while (PlaceableGrid.IsWithinBounds(GridIndex) && PlaceableGrid.GetValue(GridIndex.X, GridIndex.Y, GridIndex.Z))
	{
		GridIndex.Z++;
	}

	const float CellSize = PlaceableGrid.GetCellSize();
	const FVector NextLocalLocation = FVector(GridIndex.X * CellSize, GridIndex.Y * CellSize, GridIndex.Z * CellSize);

	return GetComponentTransform().TransformPosition(NextLocalLocation);
}

FIntVector UGridComponent::GetNextFreeZPositionGrid(const FVector& WorldLocation)
{
	const FVector LocalLocation = WorldToLocal(WorldLocation);
	const FVector RoundedLocation = PlaceableGrid.GetRoundedLocation(LocalLocation);

	FIntVector GridIndex = PlaceableGrid.LocalToGrid(RoundedLocation);
	
	while (PlaceableGrid.IsWithinBounds(GridIndex) && PlaceableGrid.GetValue(GridIndex.X, GridIndex.Y, GridIndex.Z))
	{
		GridIndex.Z++;
	}
	
	return FIntVector(GridIndex.X, GridIndex.Y, GridIndex.Z);
}

FVector UGridComponent::GridToLocalPos(const FIntVector GridPos)
{
	const float CellSize = PlaceableGrid.GetCellSize();
	const FVector LocalLocation = FVector(GridPos.X * CellSize, GridPos.Y * CellSize, GridPos.Z * CellSize);
	return LocalLocation;
}


FVector UGridComponent::GetLWorldLocationFromGridIndex(const FIntVector GridPos)
{
	const float CellSize = PlaceableGrid.GetCellSize();
	const FVector LocalLocation = FVector(GridPos.X * CellSize, GridPos.Y * CellSize, GridPos.Z * CellSize);
	return GetComponentTransform().TransformPosition(LocalLocation);
}

bool UGridComponent::IsPlaceableBlocked(TObjectPtr<APlaceable> Placeable)
{
	const auto LocalLocation = WorldToLocal(Placeable->GetActorLocation());
	const TArray<FVector> OccupiedPositions = Placeable->GetAllGridPositions(LocalLocation, Placeable->GetLocalYaw(), PlaceableGrid.GetCellSize());

	for (const FVector& Pos : OccupiedPositions)
	{
		const auto GridIndex = PlaceableGrid.LocalToGrid(Pos);
		const FIntVector AbovePos = FIntVector(GridIndex.X, GridIndex.Y, GridIndex.Z + 1);

		if (!PlaceableGrid.IsWithinBounds(AbovePos))
			continue;

		auto PlaceableAbovePtr = PlaceableGrid.GetValue(AbovePos.X, AbovePos.Y, AbovePos.Z);
		if (PlaceableAbovePtr && *PlaceableAbovePtr != Placeable)
			return true;
	}

	return false;
}

int32 UGridComponent::GetHighestOccupiedZ()
{
	return PlaceableGrid.GetHighestOccupiedZ();
}

TArray<FIntVector> UGridComponent::GetPositionsFromLevel(int Z)
{
	return PlaceableGrid.GetOccupiedPositionsAtZ(Z);
}

void UGridComponent::ShowIndicators()
{
	InstancedMeshComp->SetVisibility(true);
}

void UGridComponent::HideIndicators()
{
	InstancedMeshComp->SetVisibility(false);
}

#if !UE_BUILD_SHIPPING

void UGridComponent::DrawDebugGrid(float Duration) const
{
	if (!GetWorld())
		return;

	const float CellSize = PlaceableGrid.GetCellSize();
	const int32 Height = 0;

	for (int32 X = PlaceableGrid.GetMin().X; X <= PlaceableGrid.GetMax().X; X++)
	{
		for (int32 Y = PlaceableGrid.GetMin().Y; Y <= PlaceableGrid.GetMax().Y; Y++)
		{
			for (int32 Z = PlaceableGrid.GetMin().Z; Z <= PlaceableGrid.GetMin().Z + Height; Z++)
			{
				const bool bOccupied = PlaceableGrid.GetValue(X, Y, Z) != nullptr;
				const bool bInvalidPos = PlaceableGrid.IsInvalidSlot(FIntVector(X, Y, Z));

				const FVector LocalCellCenter = FVector(X * CellSize, Y * CellSize, Z * CellSize);
				const FVector WorldCellCenter = GetComponentTransform().TransformPosition(LocalCellCenter);

				const FColor DebugColor = bOccupied ? FColor::Red : bInvalidPos ? FColor::Silver : FColor::Green;

				DrawDebugBox(GetWorld(), WorldCellCenter, FVector(CellSize * 0.4f, CellSize * 0.4f, CellSize * 0.4f),
					GetComponentQuat(), DebugColor, false, Duration, 0);
			}
		}
	}
}
#endif

void UGridComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DropOverTiltedContainers();
	
#if !UE_BUILD_SHIPPING
	if (CVarCargoShowDebugs.GetValueOnGameThread())
		DrawDebugGrid(0.f);
#endif
}

void UGridComponent::DropOverTiltedContainers()
{
	if (ContainerFallAngle <= 0)
		return;

	TSet<AContainer*> CheckedContainers;
	TMap<AContainer*, FTransform> FallingContainers;
	for (const auto& Slot : GetOccupiedSlots())
	{
		AContainer* Container = Cast<AContainer>(Slot.Value);
		if (!IsValid(Container) || CheckedContainers.Contains(Container))
			continue;
		CheckedContainers.Add(Container);

		const FTransform Pose = Container->GetStackWorldTransform();
		const float UpDot = FVector::DotProduct(Pose.GetUnitAxis(EAxis::Z), FVector::UpVector);
		const float Tilt = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(UpDot, -1.f, 1.f)));
		// Ignore floating-point noise at exactly the configured angle.
		if (Tilt > ContainerFallAngle + KINDA_SMALL_NUMBER)
			FallingContainers.Add(Container, Pose);
	}

	// Decide the entire batch before removal callbacks rebalance the ship and rebuild the stack.
	for (const auto& Falling : FallingContainers)
	{
		AContainer* Container = Falling.Key;
		if (!IsValid(Container) || Container->OwningGridActor != this)
			continue;

		RemovePlaceableFromGrid(Container);
		Container->SetActorTransform(Falling.Value, false, nullptr, ETeleportType::TeleportPhysics);
		const FVector Downhill = Falling.Value.GetUnitAxis(EAxis::Z).GetSafeNormal2D();
		const FVector ShipVelocity = GetOwner() ? GetOwner()->GetVelocity() : FVector::ZeroVector;
		Container->FallIntoSea(Downhill, ShipVelocity);
	}
}

void UGridComponent::ClearGrid()
{
	for (auto Placeable : GetOccupiedSlots())
	{
		Placeable.Value->Destroy();
	}
    
	PlaceableGrid.Clear();
}
