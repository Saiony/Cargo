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
	
	// ContainerMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ContainerMesh"));
	// ContainerMeshComp->SetupAttachment(PivotComp);
	
	SetActorEnableCollision(false);
}

void APlaceablePreview::Initialize(TObjectPtr<APlaceable> Placeable)
{
	MimicPlaceableYaw(Placeable);	
	
	const auto Visual = Placeable->GetVisual();
	TArray<UStaticMeshComponent*> MeshComponents;
	Visual->GetComponents<UStaticMeshComponent>(MeshComponents);
	
	for (UStaticMeshComponent* SourceMesh : MeshComponents)
	{
		if (!SourceMesh || !SourceMesh->GetStaticMesh())
		{
			continue;
		}

		UStaticMeshComponent* PreviewMesh = NewObject<UStaticMeshComponent>(this);

		PreviewMesh->SetStaticMesh(SourceMesh->GetStaticMesh());

		PreviewMesh->SetRelativeTransform(SourceMesh->GetRelativeTransform());

		PreviewMesh->SetupAttachment(PivotComp);

		PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		PreviewMesh->SetCollisionProfileName("NoCollision");

		PreviewMesh->RegisterComponent();
	}
}

void APlaceablePreview::MimicPlaceableYaw(TObjectPtr<APlaceable> Placeable)
{
	PivotComp->SetRelativeRotation(FRotator(0.0f, Placeable->GetLocalYaw(), 0.0f));
}

void APlaceablePreview::SetValid()
{
	TArray<USceneComponent*> ChildComponentss;
	PivotComp->GetChildrenComponents(true, ChildComponentss);

	for (auto Component : ChildComponentss)
	{
		if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Component))
		{
			Mesh->SetMaterial(0, ValidMaterial);
		}
	}
}

void APlaceablePreview::SetInvalid()
{
	TArray<USceneComponent*> ChildComponentss;
	PivotComp->GetChildrenComponents(true, ChildComponentss);

	for (auto Component : ChildComponentss)
	{
		if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Component))
		{
			Mesh->SetMaterial(0, InvalidMaterial);
		}
	}
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

