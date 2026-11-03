// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CargoIsland.h"
#include "PuzzleIsland.generated.h"

UCLASS()
class CARGO_API APuzzleIsland : public ACargoIsland
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APuzzleIsland();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
