// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/PlaceablePreview.h"


// Sets default values
APlaceablePreview::APlaceablePreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APlaceablePreview::Initialize(TObjectPtr<APlaceable> Placeable)
{
	MimicPlaceableYaw(Placeable);
}

void APlaceablePreview::MimicPlaceableYaw(TObjectPtr<APlaceable> Placeable)
{
	SetActorRotation(FRotator(0.0f, Placeable->GetActorRotation().Yaw, 0.0f));
}

// Called when the game starts or when spawned
void APlaceablePreview::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void APlaceablePreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

