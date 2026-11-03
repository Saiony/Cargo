// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WhaleSpawner.generated.h"

class AWhale;
class UMaterialInterface;
class UStaticMeshComponent;
class USceneComponent;

/** Coordinates one whale appearance, including its water shadow and spawn delay. */
UCLASS()
class AWhaleSpawner : public AActor
{
	GENERATED_BODY()

public:
	AWhaleSpawner();
	void InitializeEncounter(const FVector& InWaterSurfaceLocation);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	/** Flat visual kept at the surface while the whale moves independently below it. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> ShadowVisual;

	/** Assign a translucent radial shadow material in the Blueprint subclass. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale Spawner|Shadow")
	TObjectPtr<UMaterialInterface> ShadowMaterial;

	/** World-space width and length of the shadow plane. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale Spawner|Shadow", meta=(ClampMin="0.0", Units="cm"))
	float ShadowSize = 1200.0f;

	/** Whale class to spawn after the shadow has been visible for the delay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale Spawner")
	TSubclassOf<AWhale> WhaleClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale Spawner", meta=(ClampMin="0.0", Units="s"))
	float ShadowLeadTime = 2.0f;

	/** Distance below the water surface where the whale begins its ascent. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale Spawner", meta=(ClampMin="0.0", Units="cm"))
	float WhaleSpawnDepth = 5000.0f;

	UFUNCTION()
	void OnWhaleDestroyed(AActor* DestroyedActor);

	void SpawnWhale();

	FVector WaterSurfaceLocation = FVector::ZeroVector;
	bool bHasWaterSurfaceLocation = false;
	FTimerHandle WhaleSpawnTimer;
	TWeakObjectPtr<AWhale> SpawnedWhale;
};
