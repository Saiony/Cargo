// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/LevelManagerSubsystem.h"

#include "DeveloperSettings/CargoSettings.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Level/CargoLevelConfig.h"

void ULevelManagerSubsystem::OpenLevel(ELevelType LevelType, TFunction<void()> OnLoaded)
{
	const auto LevelDatabase = GetDefault<UCargoSettings>()->LevelDatabase.LoadSynchronous();
	const auto LevelData = LevelDatabase->GetLevelData(LevelType);
	LevelLoadedCallback = MoveTemp(OnLoaded);
	
	const FString ExpectedPackageName = LevelData.Level.GetLongPackageName();
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this, [this, ExpectedPackageName](UWorld* LoadedWorld)
	{
		if (!LoadedWorld || LoadedWorld->GetOutermost()->GetName() != ExpectedPackageName)
		{
			return;
		}
		OnLevelLoaded();
	});
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetGameInstance(), LevelData.Level, true);
}

void ULevelManagerSubsystem::LoadStreamingLevel(ELevelType LevelType, TFunction<void()> OnLoaded)
{
	const auto LevelDatabase = GetDefault<UCargoSettings>()->LevelDatabase.LoadSynchronous();
	const auto LevelData = LevelDatabase->GetLevelData(LevelType);
	LevelLoadedCallback = MoveTemp(OnLoaded);

	bool bSuccess = false;
	const auto StreamingInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetGameInstance(), LevelData.Level, FVector(0, 0, 10000), FRotator::ZeroRotator, bSuccess);
	
	if (!bSuccess || !StreamingInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelManager: Failed to instantiate level %s"), *LevelData.Level.ToString());
		LevelLoadedCallback = {};
		return;
	}

	StreamingLevelInstances.Add(LevelType, StreamingInstance);
	if (StreamingInstance->IsLevelVisible())
	{
		OnLevelLoaded();
	}
	else
	{
		// OnLevelLoaded can fire before the level transform has been applied to its actors.
		// Wait until it is visible in the world before consumers read actor positions.
		StreamingInstance->OnLevelShown.AddDynamic(this, &ThisClass::OnLevelLoaded);
	}
}

void ULevelManagerSubsystem::UnloadLevel(ELevelType LevelType, TFunction<void()> OnUnloaded)
{ 	
	LevelUnloadedCallback = MoveTemp(OnUnloaded);
	if (TObjectPtr<ULevelStreamingDynamic>* StreamingInstance = StreamingLevelInstances.Find(LevelType))
	{
		if (IsValid(*StreamingInstance))
		{
			(*StreamingInstance)->OnLevelUnloaded.AddDynamic(this, &ThisClass::OnLevelUnloaded);
			(*StreamingInstance)->SetShouldBeVisible(false);
			(*StreamingInstance)->SetShouldBeLoaded(false);
			(*StreamingInstance)->SetIsRequestingUnloadAndRemoval(true);
			return;
		}
		StreamingLevelInstances.Remove(LevelType);
	}
	OnLevelUnloaded();
}

void ULevelManagerSubsystem::OnLevelLoaded()
{
	UE_LOG(LogTemp, Log, TEXT("LevelManager: Level loaded"));
	if (LevelLoadedCallback)
	{
		LevelLoadedCallback();
		LevelLoadedCallback = {};
	}
}

void ULevelManagerSubsystem::OnLevelUnloaded()
{
	UE_LOG(LogTemp, Log, TEXT("LevelManager: Level unloaded"));
	if (LevelUnloadedCallback)
	{
		LevelUnloadedCallback();
		LevelUnloadedCallback = {};
	}
}

void ULevelManagerSubsystem::TeleportPlayer(ELevelType LevelType)
{
	const auto World = GetGameInstance()->GetWorld();
	check(World);

	const auto TargetLevel = GetLevel(LevelType);
	check(TargetLevel);

	const auto LevelConfig = GetLevelConfig(TargetLevel);
	check(LevelConfig);

	const auto PlayerStart = LevelConfig->GetPlayerStart();
	check(PlayerStart);

	const auto PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	check(PlayerController);

	const auto PlayerPawn = PlayerController->GetPawn();
	check(PlayerPawn);

	PlayerPawn->TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
}

ULevel* ULevelManagerSubsystem::GetLevel(const ELevelType LevelType) const
{
	const TObjectPtr<ULevelStreamingDynamic>* StreamingInstance = StreamingLevelInstances.Find(LevelType);
	return StreamingInstance && IsValid(*StreamingInstance) ? (*StreamingInstance)->GetLoadedLevel() : nullptr;
}

ACargoLevelConfig* ULevelManagerSubsystem::GetLevelConfig(ULevel* Level) const
{
	for (AActor* Actor : Level->Actors)
	{
		if (ACargoLevelConfig* LevelConfig = Cast<ACargoLevelConfig>(Actor))
		{
			return LevelConfig;
		}
	}

	return nullptr;
}
