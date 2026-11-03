// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterComponents/ShipPaddle.h"

#include "CargoCharacter.h"


// Sets default values
AShipPaddle::AShipPaddle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AShipPaddle::BeginPlay()
{
	Super::BeginPlay();	
	
	Ship = Cast<ACargoCharacter>(GetOwner());
	check(Ship.IsValid());
	
	Ship->OnMovementStarted.AddUObject(this, &ThisClass::OnShipMovementStarted);
	Ship->OnMovementStopped.AddUObject(this, &ThisClass::OnShipMovementStopped);
}

void AShipPaddle::OnShipMovementStarted()
{
}

void AShipPaddle::OnShipMovementStopped()
{
}

void AShipPaddle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!Ship.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ShipPaddle: Ship is not valid"));
		return;
	}

	const float PaddleRotationSpeed = Ship->GetShipCurrentSpeed() * SpeedMultiplier;
	Pivot->AddLocalRotation(FRotator(0.0f, PaddleRotationSpeed * DeltaTime, 0.0f));
}

