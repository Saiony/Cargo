// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShipPaddle.generated.h"

class ACargoCharacter;

UCLASS()
class CARGO_API AShipPaddle : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShipPaddle();

protected:
	UPROPERTY()
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, Category="Cargo")
	TObjectPtr<USceneComponent> Pivot;
	
	UPROPERTY(EditAnywhere, Category="Cargo")
	float SpeedMultiplier = 1.0f;
	
	TWeakObjectPtr<ACargoCharacter> Ship;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnShipMovementStarted();
	
	void OnShipMovementStopped();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
