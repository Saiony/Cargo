// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/LevelManagerSubsystem.h"

#include "DeveloperSettings/CargoSettings.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ULevelManagerSubsystem::LoadLevel(ELevelType LevelType)
{
	const auto LevelDatabase = GetDefault<UCargoSettings>()->LevelDatabase.LoadSynchronous();
	const auto LevelData = LevelDatabase->GetLevelData(LevelType);
	PendingLevelData = LevelData;

	switch (LevelData.LoadType)
	{
		case ELevelLoadType::OpenLevel:
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(GetGameInstance(), LevelData.Level, true);

			break;
		}
		case ELevelLoadType::Streaming:
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			LatentInfo.ExecutionFunction = GET_FUNCTION_NAME_CHECKED(ThisClass, OnLevelLoaded);
			LatentInfo.UUID = 123;
			LatentInfo.Linkage = 0;

			UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetGameInstance(), LevelData.Level, true, false, LatentInfo);

			break;
		}
		case ELevelLoadType::Unknown:
		default:
		{
			UE_LOG(LogTemp, Error, TEXT("LevelManager: Invalid ELevelLoadType: %d"), LevelData.LoadType);
			break;
		}
	}
}

void ULevelManagerSubsystem::UnloadLevel(ELevelType LevelType)
{	
	const auto LevelDatabase = GetDefault<UCargoSettings>()->LevelDatabase.LoadSynchronous();
	const auto LevelData = LevelDatabase->GetLevelData(LevelType);
	
	if (LevelData.LoadType == ELevelLoadType::OpenLevel)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelManager: Cannot unload open level"));
		return;
	}
	
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = GET_FUNCTION_NAME_CHECKED(ThisClass, OnLevelUnloaded);
	LatentInfo.UUID = 123;
	LatentInfo.Linkage = 0;

	UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetGameInstance(), LevelData.Level, LatentInfo, true);
}

void ULevelManagerSubsystem::OnLevelLoaded()
{
	UE_LOG(LogTemp, Log, TEXT("LevelManager: Level loaded"));

}

void ULevelManagerSubsystem::OnLevelUnloaded()
{
	UE_LOG(LogTemp, Log, TEXT("LevelManager: Level unloaded"));
	
	
}

void ULevelManagerSubsystem::TeleportPlayer()
{
	const auto World = GetGameInstance()->GetWorld();
	check(World);

	const auto PlayerStart = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass());
	check(PlayerStart);

	const auto PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	check(PlayerController);

	const auto PlayerPawn = PlayerController->GetPawn();
	check(PlayerPawn);

	PlayerPawn->TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
}
