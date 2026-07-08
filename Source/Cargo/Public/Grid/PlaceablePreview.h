// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable.h"
#include "GameFramework/Actor.h"
#include "PlaceablePreview.generated.h"

UCLASS()
class CARGO_API APlaceablePreview : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlaceablePreview();
	
	void Initialize(TObjectPtr<APlaceable> Placeable);
	void MimicPlaceableYaw(TObjectPtr<APlaceable> Placeable);

protected:	
	UPROPERTY(EditAnywhere, Config, Category = "Cargo")
	TSoftObjectPtr<UMaterialInterface> PreviewMaterial;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
