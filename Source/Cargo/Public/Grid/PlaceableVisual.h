// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "DataAssets/ContainerDA.h"
#include "PlaceableVisual.generated.h"


UCLASS()
class CARGO_API APlaceableVisual : public AActor
{
	GENERATED_BODY()	
	
	void ClearMeshes();
public:
	// Sets default values for this component's properties
	APlaceableVisual();
	
	void Initialize(TObjectPtr<UContainerDA> Data);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
};
