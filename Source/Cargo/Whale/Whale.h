// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Whale.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class AAutoDestructible;

/** Scripted underwater breach sequence. This is a visual actor, not a physics pawn. */
UCLASS()
class AWhale : public AActor
{
	GENERATED_BODY()

public:
	AWhale();
	void InitializeBreach(const FVector& InWaterSurfaceLocation, float InInitialDepth);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	/** Assign the whale mesh in a Blueprint subclass. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> WhaleMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Movement", meta=(ClampMin="0.0", Units="cm"))
	float BreachHeight = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Movement", meta=(ClampMin="0.0", Units="s"))
	float RiseDuration = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Movement", meta=(ClampMin="0.0", Units="s"))
	float PeakHoldDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Movement", meta=(ClampMin="0.0", Units="s"))
	float DescendDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Movement", meta=(ClampMin="0.0", Units="cm"))
	float SubmergeDepth = 1000.0f;

	/** Optional impact visual spawned at the water surface when the whale breaches. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Impact")
	TSubclassOf<AAutoDestructible> ImpactVisualClass;

	/** Maximum roll added to the cargo ship when the whale breaks the surface. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Impact", meta=(ClampMin="0.0", Units="deg"))
	float ShipImpactRollDegrees = 35.0f;

	/** Horizontal ship velocity added away from the impact point, in cm/s. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Impact", meta=(ClampMin="0.0", Units="cm/s"))
	float ShipImpactPushStrength = 1200.0f;

	/** Maximum horizontal distance from the breach that can affect the ship. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Impact", meta=(ClampMin="0.0", Units="cm"))
	float ShipImpactRadius = 5000.0f;

	/** Maximum horizontal distance for the second impact after the whale finishes toppling. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Impact", meta=(ClampMin="0.0", Units="cm"))
	float ToppleImpactRadius = 8000.0f;

	/** Maximum angle the whale tips toward the player after reaching breach height. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Movement", meta=(ClampMin="0.0", ClampMax="85.0", Units="deg"))
	float ToppleAngle = 55.0f;

	/** Time taken for the whale to tip toward the player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Whale|Movement", meta=(ClampMin="0.0", Units="s"))
	float ToppleDuration = 0.6f;

	FVector WaterSurfaceLocation = FVector::ZeroVector;
	float InitialDepth = 5000.0f;
	float ElapsedSequenceTime = 0.0f;
	bool bHasWaterSurfaceLocation = false;
	bool bSurfaceImpactTriggered = false;
	bool bToppleStarted = false;
	bool bToppleImpactTriggered = false;
	FRotator InitialActorRotation = FRotator::ZeroRotator;
	FRotator ToppleTargetRotation = FRotator::ZeroRotator;

	void TriggerSurfaceImpact();
	void TriggerToppleImpact();
	void ApplyImpactToShip(const FVector& VisualLocation, float ImpactRadius);
};
