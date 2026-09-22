// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterComponents/CoalChimney.h"

#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Grid/PlaceablePreview.h"
#include "NiagaraComponent.h"

void ACoalChimney::BeginDropHover(APlaceable* Placeable)
{
}

void ACoalChimney::EndDropHover()
{
}

void ACoalChimney::UpdateDropHover(APlaceable* Placeable, const FVector& ImpactPoint, APlaceablePreview* Preview)
{
	Preview->SetActorHiddenInGame(true);
}

bool ACoalChimney::TryAcceptDrop(APlaceable* Placeable, APlaceablePreview* Preview)
{
	// Fuel acceptance will be implemented here.
	return false;
}

ACoalChimney::ACoalChimney()
{	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	
	SmokeNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeNiagaraComp"));	
	SmokeNiagaraComp->SetupAttachment(MeshComp);
	
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(MeshComp);
}


void ACoalChimney::BeginPlay()
{
	
}
