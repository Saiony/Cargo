#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IslandDistanceBaker.generated.h"

class ACargoIsland;
class UDataTable;
class UStaticMeshComponent;
class ANavMeshBoundsVolume;
class ANavModifierVolume;
class ARecastNavMesh;
struct FGameplayTag;

UCLASS()
class CARGOEDITOR_API AIslandDistanceBaker : public AActor
{
	GENERATED_BODY()

public:
	AIslandDistanceBaker();

	UPROPERTY(EditAnywhere, Category="Cargo|Island", meta=(RequiredAssetDataTags="RowStructure=/Script/Cargo.IslandDistanceRow"))
	TObjectPtr<UDataTable> DistanceTable;

	UPROPERTY(EditAnywhere, Category="Cargo|Island", meta=(ToolTip="Prepare and rebuild a flat ocean NavMesh before baking, using the WaterBodyOcean height and blocking terrain above sea level."))
	bool bPrepareOceanNavigation = true;

	UPROPERTY(EditAnywhere, Category="Cargo|Island", meta=(Units="cm", ToolTip="World Z used for all island navigation queries, ignoring actor height. Set this to the ocean navigation surface height."))
	float NavigationHeight = -294.484f;

	UPROPERTY(EditAnywhere, Category="Cargo|Island", meta=(ClampMin="0.0", Units="cm", ToolTip="Search half extents around each island XY at Navigation Height. Distance is measured between projected points, excluding offsets from island pivots."))
	FVector NavigationSearchExtent = FVector(100000.f, 100000.f, 50000.f);

	UPROPERTY(EditAnywhere, Category="Cargo|Island", meta=(ClampMin="0.0", ClampMax="1.0", ToolTip="Reject pairs when the combined horizontal projection offsets exceed this fraction of the direct island separation. 0.1 allows up to 10 percent."))
	float MaxProjectionErrorRatio = 0.1f;

	UFUNCTION(CallInEditor, Category="Cargo|Island")
	void RecalculateDistances();

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> NavigationSurface;

	UPROPERTY()
	TObjectPtr<ANavMeshBoundsVolume> NavigationBounds;

	UPROPERTY()
	TObjectPtr<ANavModifierVolume> LandExclusion;

	ARecastNavMesh* PrepareNavigation(const TArray<ACargoIsland*>& Islands);
	TArray<ACargoIsland*> FindCargoIslands() const;
	bool CalculateDistance(const ACargoIsland* Origin, const ACargoIsland* Destination, float& OutDistance) const;
	FName MakeRowName(const FGameplayTag& Origin, const FGameplayTag& Destination) const;
};
