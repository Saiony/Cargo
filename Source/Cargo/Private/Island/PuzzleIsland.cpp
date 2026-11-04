// Fill out your copyright notice in the Description page of Project Settings.


#include "Island/PuzzleIsland.h"


// Sets default values
APuzzleIsland::APuzzleIsland()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	IslandMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IslandMesh"));
	IslandMeshComp->SetupAttachment(RootComponent);
	
	PuzzleCargoPortComp = CreateDefaultSubobject<UPuzzleCargoPort>(TEXT("PuzzleCargoPortComp"));
	PuzzleCargoPortComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APuzzleIsland::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APuzzleIsland::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

