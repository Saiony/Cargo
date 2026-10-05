// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/LevelDatabase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManagerSubsystem.generated.h"



enum class ELevelType
{
	Unknown = 0,
	MainMenu,
	MainLevel,
	DryDock,
};


enum class ELevelLoadType
{
	Unknown = 0,
	OpenLevel,
	Streaming,
};

USTRUCT(BlueprintType)
struct FLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELevelLoadType LoadType;
};

/**
 * 
 */
UCLASS()
class CARGO_API ULevelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	FLevelData PendingLevelData;
	
	void OnLevelLoaded();
	void OnLevelUnloaded();

public:
	void LoadLevel(ELevelType LevelType);
	void UnloadLevel(ELevelType LevelType);
	
	void TeleportPlayer();
};
