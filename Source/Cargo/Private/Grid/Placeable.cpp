// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Grid/Placeable.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
APlaceable::APlaceable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void APlaceable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlaceable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlaceable::Grab()
{	
	if (OwningGridActor == nullptr)
		return;
	
	IGridActorInterface::Execute_OnPlaceableGrabbed(OwningGridActor.GetObject(), this);
}

void APlaceable::Init(TScriptInterface<IGridActorInterface> GridActor, int32 GridPosX, int32 GridPosY)
{
	OwningGridActor = GridActor;
	GridPos = FIntPoint(GridPosX, GridPosY);
}

