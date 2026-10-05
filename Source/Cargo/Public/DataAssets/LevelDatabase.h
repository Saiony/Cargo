// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelDatabase.generated.h"

struct FLevelData;
enum class ELevelType;
/**
 * 
 */
UCLASS()
class CARGO_API ULevelDatabase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TMap<ELevelType, FLevelData> Levels;
	
public:
	FLevelData GetLevelData(ELevelType LevelType) const;
};
