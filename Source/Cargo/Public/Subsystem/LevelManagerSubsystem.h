// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/LevelDatabase.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManagerSubsystem.generated.h"
/**
 * 
 */
UCLASS()
class CARGO_API ULevelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	TFunction<void()> LevelLoadedCallback;
	TFunction<void()> LevelUnloadedCallback;
	UPROPERTY()
	TMap<ELevelType, TObjectPtr<ULevelStreamingDynamic>> StreamingLevelInstances;

	ULevel* GetLevel(ELevelType LevelType) const;
	class ACargoLevelConfig* GetLevelConfig(ULevel* Level) const;


public:
	void OpenLevel(ELevelType LevelType, TFunction<void()> OnLoaded = {});
	void LoadStreamingLevel(ELevelType LevelType, TFunction<void()> OnLoaded = {});
	void UnloadLevel(ELevelType LevelType, TFunction<void()> OnUnloaded = {});
	
	void TeleportPlayer(ELevelType LevelType);
	
	UFUNCTION()
	void OnLevelLoaded();
	
	UFUNCTION()
	void OnLevelUnloaded();
};
