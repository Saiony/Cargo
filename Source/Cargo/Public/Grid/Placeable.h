// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CargoCharacter.h"
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
	UStaticMeshComponent* RootMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
	UStaticMeshComponent* ContainerMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cargo")
	TObjectPtr<UMaterialInterface> Material;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float Weight;
	
	TScriptInterface<IGridActorInterface> OwningGridActor;
	
	FIntPoint GridPos;	

protected:
	UPROPERTY(VisibleAnywhere, Category = "Placement")
	float LocalYaw = 0.f;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Placement")
	float GetLocalYaw() const { return LocalYaw; }
	
	void Grab();
	
	void Init(TScriptInterface<IGridActorInterface> GridActor, int32 GridPosX, int32 GridPosY);
	
	//Rotates the root mesh internally
	void RotateClockwise();
	void AlignToRotation(const FRotator& ReferenceRotation);
};
