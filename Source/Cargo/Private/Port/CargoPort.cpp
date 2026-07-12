// Fill out your copyright notice in the Description page of Project Settings.

#include "Port/CargoPort.h"

#include "GameplayTagContainer.h"
#include "Components/StaticMeshComponent.h"
#include "Grid/Container.h"
#include "Grid/Placeable.h"
#include "CargoGameMode.h"

UCargoPortComponent::UCargoPortComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCargoPortComponent::BeginPlay()
{
	Super::BeginPlay();

	OnPlaceableAddedToGrid.AddDynamic(this, &UCargoPortComponent::HandlePlaceableAddedToGrid);
	OnPlaceableRemovedFromGrid.AddDynamic(this, &UCargoPortComponent::HandlePlaceableRemovedFromGrid);
}

void UCargoPortComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCargoPortComponent::AttachPlaceable(APlaceable* Placeable, FVector WorldPos)
{    
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	Placeable->AttachToComponent(this, AttachmentRules);
}

void UCargoPortComponent::AddPlaceable(APlaceable* Placeable, FVector WorldPos, float Rotation)
{
	if (!IsOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Attempt to add placeable to a closed port"));
		return;
	}

	AddPlaceableToGrid(Placeable, WorldPos, Rotation);

	const FRotator WorldRotation = GetComponentRotation() + FRotator(0, Rotation, 0);

	Placeable->SetActorLocation(WorldPos, false);
	AttachPlaceable(Placeable, WorldPos);
	Placeable->SetActorRotation(WorldRotation);
}

void UCargoPortComponent::HandlePlaceableAddedToGrid(APlaceable* Placeable)
{
	if (auto Container = Cast<AContainer>(Placeable))
	{
		if (ACargoGameMode* GM = ACargoGameMode::Get(this))
		{
			GM->RegisterCargoDelivery(CurrentQuestTag, Container->ContainerDA->CargoTag);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Placeable is not a container"));
	}
}

void UCargoPortComponent::HandlePlaceableRemovedFromGrid(APlaceable* Placeable)
{
	if (!IsOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: Placeable removed while port is closed"));
	}

	Placeable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (auto Container = Cast<AContainer>(Placeable))
	{
		if (ACargoGameMode* GM = ACargoGameMode::Get(this))
		{
			GM->RemoveCargoDelivery(CurrentQuestTag, Container->ContainerDA->CargoTag);
		}
	}
}

void UCargoPortComponent::SpawnSingleContainer(FGameplayTag CargoType)
{
	const auto SoftDA = GetDefault<UCargoSettings>()->ContainersMap.Find(CargoType);
	if (!SoftDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("CargoPortComponent: No container data asset found for cargo type %s"), *CargoType.ToString());
		return;
	}

	auto ContainerDA = SoftDA->LoadSynchronous();

	constexpr float GridRelativeRotation = 0.0f;
	const FRotator SpawnRotation = GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AContainer* NewContainer = GetWorld()->SpawnActor<AContainer>(ContainerClass, GetComponentLocation(), SpawnRotation, SpawnParams);

	if (!NewContainer)
	{
		UE_LOG(LogTemp, Error, TEXT("CargoPortComponent: Failed to spawn container"));
		return;
	}

	NewContainer->Init(ContainerDA); // agora o Size já está correto ANTES de qualquer validação

	for (int32 X = PlaceableGrid.GetMin().X; X <= PlaceableGrid.GetMax().X; X++)
	{
		for (int32 Y = PlaceableGrid.GetMin().Y; Y <= PlaceableGrid.GetMax().Y; Y++)
		{
			const FVector LocalPos = PlaceableGrid.GridToLocal(FIntPoint(X, Y));
			const FVector WorldPos = GetComponentTransform().TransformPosition(LocalPos);

			if (!CanAddPlaceableToGrid(NewContainer, WorldPos, GridRelativeRotation))
				continue;

			AddPlaceable(NewContainer, WorldPos, GridRelativeRotation);

			UE_LOG(LogTemp, Log, TEXT("- Spawned %s at %s"), *NewContainer->GetName(), *WorldPos.ToString());
			return;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("CargoPortComponent: No valid grid slot found for cargo type %s"), *CargoType.ToString());
	NewContainer->Destroy();
}

void UCargoPortComponent::StartQuestDelivery(FGameplayTag QuestTag)
{
	IsOpen = true;
	CurrentQuestTag = QuestTag;	
}

void UCargoPortComponent::Clear()
{
	IsOpen = false;
	CurrentQuestTag = FGameplayTag();

	ClearGrid();
}

void UCargoPortComponent::SpawnCargo(const TArray<FCargoRequirement>& Requirements)
{
	for (const FCargoRequirement& Req : Requirements)
	{
		for (int32 i = 0; i < Req.Quantity; ++i)
		{
			SpawnSingleContainer(Req.CargoType);
		}
	}
}
