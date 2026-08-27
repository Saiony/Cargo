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
	if (PlaceableDA == nullptr)
		return;
	
	// if (PlaceableDA->VisualBPClass) //right now the class in the data is pretty much an override
	// 	PlaceableVisualComp->SetChildActorClass(PlaceableDA->VisualBPClass);
	
	if (!PlaceableVisualComp->GetChildActor())
		PlaceableVisualComp->CreateChildActor();
	
	Cast<APlaceableVisual>(PlaceableVisualComp->GetChildActor())->Initialize(PlaceableDA);
	
	Weight = PlaceableDA->Weight;
	Name = PlaceableDA->Name;	
	GridShapeDefinition = PlaceableDA->Shape;
	PlaceableTag = PlaceableDA->CargoTag;
	
	//UpdateMesh();
}

void AContainer::Init(TObjectPtr<UContainerDA> InContainerDA)
{
	PlaceableDA = InContainerDA;
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
