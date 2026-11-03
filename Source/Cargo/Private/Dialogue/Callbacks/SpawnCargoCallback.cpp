// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/Callbacks/SpawnCargoCallback.h"

#include "Components/SceneComponent.h"
#include "Grid/TransientGridComponent.h"
#include "DataAssets/GridComponentDA.h"

void USpawnCargoCallback::ExecuteCallback(UDialogueData* DialogueDefinition, ACargoGameMode* GameMode, AActor* Instigator)
{
	Super::ExecuteCallback(DialogueDefinition, GameMode, Instigator);

	const FVector SpawnOrigin = Instigator->GetActorLocation();
	const FRotator SpawnRotation = Instigator->GetActorRotation();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* GridOwner = GameMode->GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), SpawnOrigin, SpawnRotation, SpawnParameters);
	USceneComponent* GridRoot = NewObject<USceneComponent>(GridOwner, TEXT("Root"));
	GridOwner->SetRootComponent(GridRoot);
	GridRoot->RegisterComponent();

	UGridComponentDA* GridData = NewObject<UGridComponentDA>(GridOwner);
	GridData->GridSize = FIntVector(GridSizeXY.X, GridSizeXY.Y, CargoToSpawn.Quantity - 1);

	UTransientGridComponent* CargoGrid = NewObject<UTransientGridComponent>(GridOwner, TEXT("SpawnedCargoGrid"));
	GridOwner->AddInstanceComponent(CargoGrid);
	CargoGrid->SetupAttachment(GridRoot);
	CargoGrid->SetGridComponentDA(GridData);
	CargoGrid->RegisterComponent();

	for (int32 ContainerIndex = 0; ContainerIndex < CargoToSpawn.Quantity; ++ContainerIndex)
	{
		CargoGrid->SpawnSingleContainer(CargoToSpawn.CargoType);
	}

	CompleteCallback();
}
