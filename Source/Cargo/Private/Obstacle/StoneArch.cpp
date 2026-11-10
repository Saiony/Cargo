// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/StoneArch.h"

#include "CargoCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AStoneArch::AStoneArch()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
	SetRootComponent(DetectionBox);
	DetectionBox->SetBoxExtent(FVector(200.0f));
	DetectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	DetectionBox->SetGenerateOverlapEvents(true);
	DetectionBox->OnComponentBeginOverlap.AddDynamic(this, &AStoneArch::OnDetectionBoxBeginOverlap);
}

// Called when the game starts or when spawned
void AStoneArch::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AStoneArch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStoneArch::OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("StoneArch::OnDetectionBoxBeginOverlap"));
	
	const auto Placeable = Cast<APlaceable>(OtherActor);
	
	if (!Placeable)
		return;
	
	const auto Direction = GetActorForwardVector();
	Placeable->FallIntoSea(Direction);	
}
