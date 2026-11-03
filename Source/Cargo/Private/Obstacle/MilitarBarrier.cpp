// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/MilitarBarrier.h"

#include "CargoCharacter.h"
#include "Components/BoxComponent.h"
#include "Subsystem/FROGDialogueSubsystem.h"
#include "Engine/GameInstance.h"

// Sets default values
AMilitarBarrier::AMilitarBarrier()
{
	PrimaryActorTick.bCanEverTick = false;

	DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
	SetRootComponent(DetectionBox);
	DetectionBox->SetBoxExtent(FVector(200.0f));
	DetectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	DetectionBox->SetGenerateOverlapEvents(true);
	DetectionBox->OnComponentBeginOverlap.AddDynamic(this, &AMilitarBarrier::OnDetectionBoxBeginOverlap);
	DetectionBox->OnComponentEndOverlap.AddDynamic(this, &AMilitarBarrier::OnDetectionBoxEndOverlap);
		
	BlockingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox"));
	BlockingBox->SetupAttachment(RootComponent);
	BlockingBox->SetBoxExtent(FVector(100.0f));
	BlockingBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlockingBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	BlockingBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	BlockingBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	BlockingBox->SetGenerateOverlapEvents(false);
}

// Called when the game starts or when spawned
void AMilitarBarrier::BeginPlay()
{
	Super::BeginPlay();
	BlockingBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMilitarBarrier::OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto PlayerCharacter = Cast<ACargoCharacter>(OtherActor);
	if (!PlayerCharacter || PlayerCharacter == DetectedPlayer.Get())
		return;

	DetectedPlayer = PlayerCharacter;
	
	const ACargoPlayerState* PlayerState = PlayerCharacter->GetPlayerState<ACargoPlayerState>();
	if (!PlayerState)
	{
		return;
	}

	//player can pass
	if (PlayerState->GetPatent() >= RequiredPatent)
	{
		BlockingBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}

	BlockingBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	const auto DialogueSubsystem = GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>();
	DialogueSubsystem->PlayDialogue(DialogueData, OtherActor);
}

void AMilitarBarrier::OnDetectionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == DetectedPlayer.Get() && !DetectionBox->IsOverlappingActor(OtherActor))
	{
		DetectedPlayer.Reset();
		BlockingBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

