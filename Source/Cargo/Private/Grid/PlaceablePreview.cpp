// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/PlaceablePreview.h"


// Sets default values
APlaceablePreview::APlaceablePreview()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
	
	PivotComp = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	PivotComp->SetupAttachment(RootComponent);
	
	ContainerMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ContainerMesh"));
	ContainerMeshComp->SetupAttachment(PivotComp);
	
	SetActorEnableCollision(false);
}

void APlaceablePreview::Initialize(TObjectPtr<APlaceable> Placeable)
{
	MimicPlaceableYaw(Placeable);
}

void APlaceablePreview::MimicPlaceableYaw(TObjectPtr<APlaceable> Placeable)
{
	SetActorRotation(Placeable->GetActorRotation());
	PivotComp->SetRelativeRotation(FRotator(0.0f, Placeable->GetLocalYaw(), 0.0f));
}

void APlaceablePreview::SetValid()
{
	ContainerMeshComp->SetMaterial(0, ValidMaterial);
}

void APlaceablePreview::SetInvalid()
{
	ContainerMeshComp->SetMaterial(0, InvalidMaterial);
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

