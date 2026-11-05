// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Port/PuzzleCargoPort.h"
#include "PuzzleIsland.generated.h"

UINTERFACE()
class UPuzzleCargoPortListener : public UInterface
{
	GENERATED_BODY()
};

class IPuzzleCargoPortListener
{
	GENERATED_BODY()

public:
	virtual void OnPuzzleFinished() = 0;
};

UCLASS()
class CARGO_API APuzzleIsland : public AActor, public IPuzzleCargoPortListener
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	TObjectPtr<UStaticMeshComponent> IslandMeshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UPuzzleCargoPort> PuzzleCargoPortComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Cargo")
	TObjectPtr<UContainerDA> RewardContainer;	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	APuzzleIsland();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnPuzzleFinished() override;	
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPuzzleFinished_BP();
};
