// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WhaleArea.generated.h"

class AWaterBody;
class AWhaleSpawner;
class APawn;
class UBoxComponent;

/** Detects the player and periodically creates whale appearances nearby. */
UCLASS()
class AWhaleArea : public AActor
{
	GENERATED_BODY()

public:
	AWhaleArea();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> DetectionVolume;

	/** Half-size of the editable player detection box, in centimeters. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Whale Area", meta=(ClampMin="0.0", Units="cm"))
	FVector DetectionBoxExtent = FVector(2000.0f, 2000.0f, 1000.0f);

	/** Encounter actor that displays the shadow and spawns a whale. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Whale Area")
	TSubclassOf<AWhaleSpawner> WhaleSpawnerClass;

	/** Time between whale appearances while a player remains inside the box. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Whale Area", meta=(ClampMin="0.1", Units="s"))
	float SpawnInterval = 5.0f;

	/** Minimum horizontal distance from the player for a whale encounter. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Whale Area|Spawn", meta=(ClampMin="0.0", Units="cm"))
	float MinSpawnRadius = 1500.0f;

	/** Maximum horizontal distance from the player for a whale encounter. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Whale Area|Spawn", meta=(ClampMin="0.0", Units="cm"))
	float MaxSpawnRadius = 2500.0f;

	/** Half-angle of the spawn arc around the player's forward direction. Smaller values keep whales more directly ahead. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Whale Area|Spawn", meta=(ClampMin="0.0", ClampMax="90.0", Units="deg"))
	float FrontSpawnHalfAngle = 25.0f;

	/** Optional water body. If unset, the area searches for a water body in the level. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Whale Area|Water")
	TObjectPtr<AWaterBody> WaterBody;

	/** Surface height used if no Water Body can provide a surface query. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Whale Area|Water", meta=(Units="cm"))
	float FallbackWaterSurfaceZ = 0.0f;

	UFUNCTION()
	void OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	void StartSpawnTimer(APawn* PlayerPawn);
	void StopSpawnTimer();
	void SpawnWhaleSpawner();
	bool FindWaterSurface(const FVector& NearLocation, FVector& OutSurfaceLocation) const;

	TWeakObjectPtr<APawn> ActivePlayerPawn;
	FTimerHandle SpawnTimer;
};
