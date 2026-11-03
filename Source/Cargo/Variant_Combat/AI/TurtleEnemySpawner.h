// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurtleEnemySpawner.generated.h"

class USceneComponent;
class USplineComponent;
class ATurtleEnemy;

/** Spawns one turtle that follows this actor's editable spline. */
UCLASS()
class ATurtleEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	ATurtleEnemySpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	/** Path assigned to the spawned turtle. Edit spline points in the level editor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USplineComponent> MovementSpline;

	/** Character class to spawn. Use ATurtleEnemy or a Blueprint subclass. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Turtle Spawner")
	TSubclassOf<ATurtleEnemy> TurtleClass;

	/** Spawn the turtle automatically when play begins. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Turtle Spawner")
	bool bSpawnAtBeginPlay = true;

	/** Seconds between automatic turtle spawns. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Turtle Spawner", meta=(ClampMin="0.1", Units="s"))
	float SpawnInterval = 5.0f;

	FTimerHandle SpawnTimer;
	void SpawnTurtleOnTimer();

public:
	UFUNCTION(BlueprintCallable, Category="Turtle Spawner")
	ATurtleEnemy* SpawnTurtle();
};
