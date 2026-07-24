// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/Container.h"


// Sets default values
AContainer::AContainer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AContainer::Init()
{
	if (ContainerDA == nullptr)
		return;
	
	ContainerMeshComp->SetMaterial(0, ContainerDA->Material);
	Weight = ContainerDA->Weight;
}

void AContainer::Init(TObjectPtr<UContainerDA> InContainerDA)
{
	Size = InContainerDA->Size;
	ContainerDA = InContainerDA;
	Init();
}

// Called when the game starts or when spawned
void AContainer::BeginPlay()
{
	Super::BeginPlay();
}

void AContainer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Init();
}

// Called every frame
void AContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

