// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid/Visuals/BasePlaceableVisual.h"
#include "GridBasedPlaceableVisual.generated.h"

class UContainerDA;
class UStaticMeshComponent;

UCLASS()
class CARGO_API AGridBasedPlaceableVisual : public ABasePlaceableVisual
{
	GENERATED_BODY()

	void ClearMeshes();

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> GeneratedMeshes;

public:
	virtual void Initialize(TObjectPtr<UContainerDA> Data) override;
};
