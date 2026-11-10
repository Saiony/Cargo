// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePlaceableVisual.generated.h"

class UContainerDA;

UCLASS()
class CARGO_API ABasePlaceableVisual : public AActor
{
	GENERATED_BODY()

public:
	ABasePlaceableVisual();

	virtual void Initialize(TObjectPtr<UContainerDA> Data);
};
