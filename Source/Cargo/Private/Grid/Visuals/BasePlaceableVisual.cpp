// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid/Visuals/BasePlaceableVisual.h"

#include "DataAssets/ContainerDA.h"
#include "Components/SceneComponent.h"

ABasePlaceableVisual::ABasePlaceableVisual()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
}

void ABasePlaceableVisual::Initialize(TObjectPtr<UContainerDA>)
{
}
