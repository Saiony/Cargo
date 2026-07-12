// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridComponent.h"
#include "GameFramework/Actor.h"
#include "Placeable.generated.h"

UCLASS()
class CARGO_API APlaceable : public AActor
{
	GENERATED_BODY()

protected:
	APlaceable();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
	TObjectPtr<USceneComponent> RootComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
	TObjectPtr<USceneComponent> PivotComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
	TObjectPtr<UStaticMeshComponent> ContainerMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
	TObjectPtr<UMaterialInterface> Material;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float Weight;
	
	TObjectPtr<UGridComponent> OwningGridActor;
		
	FVector2D Size;	

protected:
	UPROPERTY(VisibleAnywhere, Category = "Placement")
	float LocalYaw = 0.f;
	
	FIntPoint PivotGridPos;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Placement")
	float GetLocalYaw() const { return LocalYaw; }
	
	void Grab();
	
	void Init(TObjectPtr<UGridComponent> GridActor, int32 GridPosX, int32 GridPosY);
	
	//Rotates the root mesh internally
	void RotateClockwise();
	void AlignToRotation(const FRotator& ReferenceRotation);
	
	TArray<FVector> GetAllGridPositions(const FVector& BaseLocation, float Rotation, float CellSize) const;
	
	FIntPoint GetGridPos() const { return PivotGridPos; }
};
