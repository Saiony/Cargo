#include "Island/IslandDistanceBaker.h"

#include "Builders/CubeBuilder.h"
#include "Components/BrushComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Model.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "Island/CargoIsland.h"
#include "LandscapeProxy.h"
#include "LandscapeHeightfieldCollisionComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavModifierVolume.h"
#include "NavigationSystem.h"
#include "WaterBodyOceanActor.h"
#include "WaterBodyComponent.h"

namespace
{
	void SetVolumeBounds(AVolume* Volume, const FBox& Bounds)
	{
		Volume->Modify();
		Volume->bIsEditorOnlyActor = true;
		Volume->SetActorTransform(FTransform(Bounds.GetCenter()));
		if (!Volume->Brush)
		{
			Volume->Brush = NewObject<UModel>(Volume, NAME_None, RF_Transactional);
			Volume->Brush->Initialize(nullptr, true);
		}
		Volume->GetBrushComponent()->Brush = Volume->Brush;
		UCubeBuilder* Builder = NewObject<UCubeBuilder>();
		const FVector Size = Bounds.GetSize();
		Builder->X = Size.X;
		Builder->Y = Size.Y;
		Builder->Z = Size.Z;
		Builder->Build(Volume->GetWorld(), Volume);
		Volume->SetActorLocation(Bounds.GetCenter());
		Volume->GetBrushComponent()->BuildSimpleBrushCollision();
		Volume->PostEditChange();
		FNavigationSystem::UpdateActorData(*Volume);
	}
}

ARecastNavMesh* AIslandDistanceBaker::PrepareNavigation(const TArray<ACargoIsland*>& Islands)
{
	UWorld* World = GetWorld();
	UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavigationSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island distance baker: Navigation System is unavailable."));
		return nullptr;
	}
	if (!bPrepareOceanNavigation)
		return Cast<ARecastNavMesh>(NavigationSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate));

	Modify();
	for (TActorIterator<AWaterBodyOcean> It(World); It; ++It)
	{
		NavigationHeight = It->GetWaterBodyComponent()->GetComponentLocation().Z;
		It->GetWaterBodyComponent()->Modify();
		It->GetWaterBodyComponent()->SetCanEverAffectNavigation(false);
		for (UPrimitiveComponent* Collision : It->GetWaterBodyComponent()->GetCollisionComponents())
			Collision->SetCanEverAffectNavigation(false);
		break;
	}

	FBox Bounds(ForceInit);
	for (const ACargoIsland* Island : Islands)
		Bounds += Island->GetActorLocation();
	for (TActorIterator<ALandscapeProxy> It(World); It; ++It)
	{
		Bounds += It->GetComponentsBoundingBox(true);
		It->Modify();
		It->bUsedForNavigation = true;
		It->bFillCollisionUnderLandscapeForNavmesh = true;
		for (ULandscapeHeightfieldCollisionComponent* Collision : It->CollisionComponents)
		{
			Collision->Modify();
			Collision->SetCanEverAffectNavigation(true);
			Collision->bFillCollisionUnderneathForNavmesh = true;
			FNavigationSystem::UpdateComponentData(*Collision);
		}
	}
	Bounds = Bounds.ExpandBy(FVector(20000.f, 20000.f, 0.f));

	if (!NavigationSurface)
	{
		NavigationSurface = NewObject<UStaticMeshComponent>(this, TEXT("OceanNavigationSurface"), RF_Transactional);
		AddInstanceComponent(NavigationSurface);
		NavigationSurface->SetupAttachment(GetRootComponent());
		NavigationSurface->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));
		NavigationSurface->SetCollisionProfileName(TEXT("BlockAll"));
		NavigationSurface->SetGenerateOverlapEvents(false);
		NavigationSurface->SetVisibility(false);
		NavigationSurface->SetCanEverAffectNavigation(true);
		NavigationSurface->RegisterComponent();
	}
	NavigationSurface->Modify();
	NavigationSurface->SetWorldTransform(FTransform(FQuat::Identity, FVector(Bounds.GetCenter().X, Bounds.GetCenter().Y, NavigationHeight - 50.f), FVector(Bounds.GetSize().X / 100.f, Bounds.GetSize().Y / 100.f, 1.f)));
	FNavigationSystem::UpdateComponentData(*NavigationSurface);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.OverrideLevel = GetLevel();
	if (!IsValid(NavigationBounds))
	{
		NavigationBounds = World->SpawnActor<ANavMeshBoundsVolume>(SpawnParameters);
		NavigationBounds->SetActorLabel(TEXT("Island Bake - Ocean Bounds"));
	}
	SetVolumeBounds(NavigationBounds, FBox(FVector(Bounds.Min.X, Bounds.Min.Y, NavigationHeight - 200.f), FVector(Bounds.Max.X, Bounds.Max.Y, NavigationHeight + 500.f)));
	NavigationSystem->OnNavigationBoundsUpdated(NavigationBounds);

	if (!IsValid(LandExclusion))
	{
		LandExclusion = World->SpawnActor<ANavModifierVolume>(SpawnParameters);
		LandExclusion->SetActorLabel(TEXT("Island Bake - Above Sea Exclusion"));
	}
	LandExclusion->SetAreaClass(UNavArea_Null::StaticClass());
	// Leave room for Recast's vertical voxel rounding above the flat sea surface.
	SetVolumeBounds(LandExclusion, FBox(FVector(Bounds.Min.X, Bounds.Min.Y, NavigationHeight + 30.f), FVector(Bounds.Max.X, Bounds.Max.Y, FMath::Max(Bounds.Max.Z, NavigationHeight + 1000.f) + 1000.f)));

	ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavigationSystem->GetDefaultNavDataInstance(FNavigationSystem::Create));
	if (!NavMesh)
		return nullptr;
	NavMesh->Modify();
	NavMesh->AgentMaxSlope = 1.f;
	NavMesh->TileSizeUU = 4096.f;
	for (uint8 Index = 0; Index < static_cast<uint8>(ENavigationDataResolution::MAX); ++Index)
	{
		const auto Resolution = static_cast<ENavigationDataResolution>(Index);
		NavMesh->SetCellSize(Resolution, 100.f);
		NavMesh->SetCellHeight(Resolution, 10.f);
		NavMesh->SetAgentMaxStepHeight(Resolution, 10.f);
	}
	NavigationSearchExtent.Z = 50.f;
	NavigationSystem->Build();
	NavMesh->EnsureBuildCompletion();
	UE_LOG(LogTemp, Log, TEXT("Island distance baker: ocean NavMesh built at Z=%.1f, bounds %s."), NavigationHeight, *Bounds.ToString());
	return NavMesh;
}
