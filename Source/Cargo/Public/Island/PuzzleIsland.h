// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Port/PuzzleCargoPort.h"
#include "PuzzleIsland.generated.h"

UCLASS()
class CARGO_API APuzzleIsland : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	TObjectPtr<UStaticMeshComponent> IslandMeshComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TObjectPtr<UPuzzleCargoPort> PuzzleCargoPortComp;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	APuzzleIsland();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
