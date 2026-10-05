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
	const auto LevelManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULevelManagerSubsystem>();
	
	LevelManagerSubsystem->LoadLevel(ELevelType::DryDock);
}


