// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterComponents/CoalChimney.h"

#include "Components/AudioComponent.h"

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