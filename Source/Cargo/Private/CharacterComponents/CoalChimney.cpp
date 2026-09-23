// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterComponents/CoalChimney.h"
#include "CargoCharacter.h"

#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "Grid/Container.h"
#include "Kismet/GameplayStatics.h"

ACoalChimney::ACoalChimney()
{	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComp"));
	SetRootComponent(RootSceneComp);
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	
	SmokeNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeNiagaraComp"));	
	SmokeNiagaraComp->SetupAttachment(MeshComp);
	SmokeNiagaraComp->SetAutoActivate(false);
	
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(MeshComp);
}

void ACoalChimney::BeginPlay()
{
	Super::BeginPlay();
	// Blueprints may retain a previously saved Auto Activate value.
	SmokeNiagaraComp->DeactivateImmediate();
	SmokeNiagaraComp->SetVariableFloat(TEXT("User.SmokeIntensity"), 0.f);

	Ship = Cast<ACargoCharacter>(GetParentActor());
	if (!Ship.IsValid())
		return;

	Ship->OnMovementStarted.AddUObject(this, &ThisClass::PlaySmoke);
	Ship->OnMovementStopped.AddUObject(this, &ThisClass::StopSmoke);
	if (Ship->IsShipMoving())
		PlaySmoke();
}

void ACoalChimney::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Ship.IsValid())
	{
		Ship->OnMovementStarted.RemoveAll(this);
		Ship->OnMovementStopped.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ACoalChimney::PlaySmoke()
{
	if (!SmokeNiagaraComp->IsActive())
		SmokeNiagaraComp->Activate();
	SetSmokeTarget(1.f);
}

void ACoalChimney::StopSmoke()
{
	SetSmokeTarget(0.f);
}

void ACoalChimney::SetSmokeTarget(float Intensity)
{
	TargetSmokeIntensity = Intensity;
	if (SmokeFadeDuration <= 0.f)
	{
		SmokeIntensity = TargetSmokeIntensity;
		SmokeNiagaraComp->SetVariableFloat(TEXT("User.SmokeIntensity"), SmokeIntensity);
	}
	SetActorTickEnabled(SmokeIntensity != TargetSmokeIntensity);
}

void ACoalChimney::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SmokeIntensity = FMath::FInterpConstantTo(SmokeIntensity, TargetSmokeIntensity,
		DeltaSeconds, SmokeFadeDuration > 0.f ? 1.f / SmokeFadeDuration : 0.f);
	if (SmokeFadeDuration <= 0.f)
		SmokeIntensity = TargetSmokeIntensity;
	SmokeNiagaraComp->SetVariableFloat(TEXT("User.SmokeIntensity"), SmokeIntensity);
	if (SmokeIntensity == TargetSmokeIntensity)
		SetActorTickEnabled(false);
}

void ACoalChimney::BeginDropHover(APlaceable* Placeable)
{	
	OriginalMaterial = MeshComp->GetMaterial(0);
	Placeable->PlaceableTag == CoalTag ? MeshComp->SetMaterial(0, HoverMaterial_Right) : MeshComp->SetMaterial(0, HoverMaterial_Wrong);	
	OnStartHover();
}

void ACoalChimney::EndDropHover()
{
	MeshComp->SetMaterial(0, OriginalMaterial);
	OnEndHover();
}

void ACoalChimney::UpdateDropHover(APlaceable* Placeable, const FVector& ImpactPoint, APlaceablePreview* Preview)
{
}

bool ACoalChimney::TryAcceptDrop(APlaceable* Placeable, APlaceablePreview* Preview)
{
	if (Placeable->PlaceableTag != CoalTag)
		return false;
	
	Placeable->Destroy();
	const auto PlayerState = Cast<ACargoPlayerState>(UGameplayStatics::GetPlayerState(this, 0));
	PlayerState->GetFuelDomain()->AddFuel(FuelPerCoal);
	
	AudioComp->Play();
	
	OnCoalAdded();
	
	return true;
}
