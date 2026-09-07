#include "Island/IslandDistanceBaker.h"

#include "Components/SceneComponent.h"
#include "DataTableUtils.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Island/CargoIsland.h"
#include "Island/IslandDistanceRow.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavigationSystem.h"
#include "Misc/ScopedSlowTask.h"
#include "ScopedTransaction.h"

AIslandDistanceBaker::AIslandDistanceBaker()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsEditorOnlyActor = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
}

void AIslandDistanceBaker::RecalculateDistances()
{
	if (!DistanceTable || DistanceTable->GetRowStruct() != FIslandDistanceRow::StaticStruct())
	{
		UE_LOG(LogTemp, Warning, TEXT("Island distance baker: select a DataTable using IslandDistanceRow."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World || World->WorldType != EWorldType::Editor)
		return;

	const TArray<ACargoIsland*> Islands = FindCargoIslands();
	TSet<FGameplayTag> IslandTags;
	for (const ACargoIsland* Island : Islands)
	{
		const FGameplayTag Tag = Island->GetLocationTag();
		if (!Tag.IsValid() || IslandTags.Contains(Tag))
		{
			UE_LOG(LogTemp, Warning, TEXT("Island distance baker: island %s has an invalid or duplicate LocationTag (%s). Table unchanged."), *Island->GetName(), *Tag.ToString());
			return;
		}
		IslandTags.Add(Tag);
	}

	const FScopedTransaction Transaction(NSLOCTEXT("IslandDistanceBaker", "RecalculateDistances", "Recalculate Island Distances"));
	FScopedSlowTask Progress(1.f, NSLOCTEXT("IslandDistanceBaker", "Baking", "Building ocean navigation and island distances..."));
	if (!IsRunningCommandlet())
		Progress.MakeDialog();
	if (!Islands.IsEmpty() && !PrepareNavigation(Islands))
	{
		UE_LOG(LogTemp, Warning, TEXT("Island distance baker: could not prepare navigation. Table unchanged."));
		return;
	}
	DistanceTable->Modify();
	DistanceTable->EmptyTable();

	for (int32 OriginIndex = 0; OriginIndex < Islands.Num(); ++OriginIndex)
	{
		for (int32 DestinationIndex = OriginIndex + 1; DestinationIndex < Islands.Num(); ++DestinationIndex)
		{
			const ACargoIsland* Origin = Islands[OriginIndex];
			const ACargoIsland* Destination = Islands[DestinationIndex];
			FIslandDistanceRow Row;
			Row.OriginIsland = Origin->GetLocationTag();
			Row.DestinationIsland = Destination->GetLocationTag();
			
			float Distance = -1;
			if (!CalculateDistance(Origin, Destination, Distance))
			{
				UE_LOG(LogTemp, Warning, TEXT("Island distance baker: could not calculate a reliable sea distance between %s and %s. Pair skipped."), *Row.OriginIsland.ToString(), *Row.DestinationIsland.ToString());
				continue;
			}

			Row.Distance = static_cast<int32>(Distance / 1000);
			const FName RowName = MakeRowName(Row.OriginIsland, Row.DestinationIsland);
			if (DistanceTable->GetRowMap().Contains(RowName))
			{
				UE_LOG(LogTemp, Warning, TEXT("Island distance baker: row name collision for %s and %s. Pair skipped."), *Row.OriginIsland.ToString(), *Row.DestinationIsland.ToString());
				continue;
			}
			DistanceTable->AddRow(RowName, Row);
		}
	}

	DistanceTable->MarkPackageDirty();
	UE_LOG(LogTemp, Log, TEXT("Island distance baker: generated %d pairs from %d islands in %s (Unreal units)."), DistanceTable->GetRowMap().Num(), Islands.Num(), *DistanceTable->GetName());
}

TArray<ACargoIsland*> AIslandDistanceBaker::FindCargoIslands() const
{
	TArray<ACargoIsland*> Islands;
	for (TActorIterator<ACargoIsland> It(GetWorld()); It; ++It)
		Islands.Add(*It);

	Islands.Sort([](const ACargoIsland& A, const ACargoIsland& B)
	{
		return A.GetLocationTag().ToString() < B.GetLocationTag().ToString();
	});
	return Islands;
}

bool AIslandDistanceBaker::CalculateDistance(const ACargoIsland* Origin, const ACargoIsland* Destination, float& OutDistance) const
{
	UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	const ANavigationData* NavData = NavigationSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	FVector OriginPosition = Origin->GetActorLocation();
	FVector DestinationPosition = Destination->GetActorLocation();
	OriginPosition.Z = NavigationHeight;
	DestinationPosition.Z = NavigationHeight;

	FNavLocation OriginLocation;
	FNavLocation DestinationLocation;
	if (!NavigationSystem->ProjectPointToNavigation(OriginPosition, OriginLocation, NavigationSearchExtent, NavData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Island distance baker: no NavMesh near %s at %s."), *Origin->GetLocationTag().ToString(), *OriginPosition.ToString());
		return false;
	}
	if (!NavigationSystem->ProjectPointToNavigation(DestinationPosition, DestinationLocation, NavigationSearchExtent, NavData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Island distance baker: no NavMesh near %s at %s."), *Destination->GetLocationTag().ToString(), *DestinationPosition.ToString());
		return false;
	}

	const double IslandSeparation = FVector::Dist2D(OriginPosition, DestinationPosition);
	const double OriginOffset = FVector::Dist2D(OriginPosition, OriginLocation.Location);
	const double DestinationOffset = FVector::Dist2D(DestinationPosition, DestinationLocation.Location);
	UE_LOG(LogTemp, Log, TEXT("Island distance baker: %s fixed-height query [%s] -> NavMesh [%s] (XY offset %.1f); %s fixed-height query [%s] -> NavMesh [%s] (XY offset %.1f)."),
		*Origin->GetLocationTag().ToString(), *OriginPosition.ToString(), *OriginLocation.Location.ToString(), OriginOffset,
		*Destination->GetLocationTag().ToString(), *DestinationPosition.ToString(), *DestinationLocation.Location.ToString(), DestinationOffset);

	if (IslandSeparation <= KINDA_SMALL_NUMBER || OriginLocation.Location.Equals(DestinationLocation.Location, 1.f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Island distance baker: overlapping island positions or projected endpoints. Check actor pivots and ocean NavMesh coverage."));
		return false;
	}
	if (OriginOffset + DestinationOffset > IslandSeparation * MaxProjectionErrorRatio)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island distance baker: combined projection offset %.1f exceeds %.1f%% of island separation %.1f. Extend the ocean NavMesh closer to the islands."), OriginOffset + DestinationOffset, MaxProjectionErrorRatio * 100.f, IslandSeparation);
		return false;
	}

	FSharedNavQueryFilter Filter = NavData->GetDefaultQueryFilter()->GetCopy();
	Filter->SetMaxSearchNodes(262144);
	FPathFindingQuery Query(this, *NavData, OriginLocation.Location, DestinationLocation.Location, Filter);
	Query.SetAllowPartialPaths(false);
	const FPathFindingResult Result = NavigationSystem->FindPathSync(Query);
	if (!Result.IsSuccessful() || !Result.Path.IsValid() || Result.Path->IsPartial())
	{
		UE_LOG(LogTemp, Warning, TEXT("Island distance baker: no complete sea path (search limit reached: %s)."), Result.Path.IsValid() && Result.Path->DidSearchReachedLimit() ? TEXT("yes") : TEXT("no"));
		return false;
	}

	const TArray<FNavPathPoint>& Points = Result.Path->GetPathPoints();
	double Distance = 0.0;
	for (int32 Index = 1; Index < Points.Num(); ++Index)
		Distance += FVector::Dist2D(Points[Index - 1].Location, Points[Index].Location);
	OutDistance = static_cast<float>(Distance);
	UE_LOG(LogTemp, Log, TEXT("Island distance baker: %s -> %s = %.1f cm (%d path points)."), *Origin->GetLocationTag().ToString(), *Destination->GetLocationTag().ToString(), OutDistance, Points.Num());
	return true;
}

FName AIslandDistanceBaker::MakeRowName(const FGameplayTag& Origin, const FGameplayTag& Destination) const
{
	return DataTableUtils::MakeValidName(Origin.ToString() + TEXT(" - ") + Destination.ToString());
}
