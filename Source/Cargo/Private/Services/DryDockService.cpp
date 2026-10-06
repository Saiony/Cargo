// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/DryDockService.h"

#include "Subsystem/LevelManagerSubsystem.h"

UDryDockService::UDryDockService()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UDryDockService::BeginPlay()
{
	Super::BeginPlay();
}


void UDryDockService::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDryDockService::GoToDryDock(TObjectPtr<ACargoIsland> From)
{
	if (!IsValid(From))
	{
		UE_LOG(LogTemp, Error, TEXT("DryDockService: Cannot travel to Dry Dock without a valid source island"));
		return;
	}

	const auto SourceWorld = From->GetWorld();
	const auto GameInstance =SourceWorld->GetGameInstance();
	const auto LevelManagerSubsystem = GameInstance->GetSubsystem<ULevelManagerSubsystem>();
	check(LevelManagerSubsystem);

	TWeakObjectPtr<ULevelManagerSubsystem> WeakLevelManagerSubsystem(LevelManagerSubsystem);
	LevelManagerSubsystem->LoadStreamingLevel(ELevelType::DryDock, [WeakLevelManagerSubsystem]()
	{
		UE_LOG(LogTemp, Log, TEXT("DryDockService: Teleporting to Dry Dock"));
		ULevelManagerSubsystem* Subsystem = WeakLevelManagerSubsystem.Get();
		Subsystem->TeleportPlayer(ELevelType::DryDock);
	});
}


