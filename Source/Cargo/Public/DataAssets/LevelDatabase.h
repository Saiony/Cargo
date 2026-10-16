// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelDatabase.generated.h"

UENUM(BlueprintType)
enum class ELevelType : uint8
{
	Unknown = 0,
	MainMenu,
	MainLevel,
	DryDock,
};

USTRUCT(BlueprintType)
struct FLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;
};

UCLASS()
class CARGO_API ULevelDatabase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TMap<ELevelType, FLevelData> Levels;
	
public:
	FLevelData GetLevelData(ELevelType LevelType) const;
};
