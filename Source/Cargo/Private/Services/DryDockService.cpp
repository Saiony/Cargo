// Fill out your copyright notice in the Description page of Project Settings.


#include "Services/DryDockService.h"

#include "CargoGameMode.h"
#include "DataAssets/ShipUpgrades/ShipUpgradeCategoryDA.h"
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

void UDryDockService::SetSelectedCategory(UShipUpgradeCategoryDA* Category)
{
	SelectedCategory = Category;
}

UCameraRigAsset* UDryDockService::GetSelectedCameraRig() const
{
	return SelectedCategory ? SelectedCategory->CameraRig.Get() : nullptr;
}

void UDryDockService::GoToDryDock(TObjectPtr<ACargoIsland> From)
{
	if (!IsValid(From))
	{
		UE_LOG(LogTemp, Error, TEXT("DryDockService: Cannot travel to Dry Dock without a valid source island"));
		return;
	}

	PreviousIsland = From;

	ACargoGameMode::Get(GetOwner())->UIService->FadeIn(FadeDuration, [this]()
	{
		const auto GameInstance = GetOwner()->GetWorld()->GetGameInstance();
		const auto LevelManagerSubsystem = GameInstance->GetSubsystem<ULevelManagerSubsystem>();

		TWeakObjectPtr<ULevelManagerSubsystem> WeakLevelManagerSubsystem(LevelManagerSubsystem);
		LevelManagerSubsystem->LoadStreamingLevel(ELevelType::DryDock, [WeakLevelManagerSubsystem, this]()
		{
			UE_LOG(LogTemp, Log, TEXT("DryDockService: Teleporting to Dry Dock"));
			ULevelManagerSubsystem* Subsystem = WeakLevelManagerSubsystem.Get();
			Subsystem->TeleportPlayer(ELevelType::DryDock);

			ACargoGameMode::Get(GetOwner())->UIService->FadeOut(FadeDuration, nullptr);
		});
	});
}

void UDryDockService::LeaveDryDock()
{
	check(PreviousIsland);
	SelectedCategory = nullptr;

	ACargoGameMode::Get(GetOwner())->UIService->FadeIn(FadeDuration, [this]()
	{
		//unload drydock level
		const auto GameInstance = GetOwner()->GetWorld()->GetGameInstance();
		const auto LevelManagerSubsystem = GameInstance->GetSubsystem<ULevelManagerSubsystem>();
		LevelManagerSubsystem->UnloadLevel(ELevelType::DryDock, nullptr);

		//get spawn location
		const auto PlayerController = GetOwner()->GetGameInstance()->GetPrimaryPlayerController();
		const auto PlayerPawn = PlayerController->GetPawn();
		const auto SpawnLocation = PreviousIsland->GetPort()->GetPlayerSpawnLocation();

		PlayerPawn->TeleportTo(SpawnLocation.GetLocation(), SpawnLocation.GetRotation().Rotator());
		ACargoGameMode::Get(GetOwner())->UIService->FadeOut(FadeDuration, nullptr);
	});
}
