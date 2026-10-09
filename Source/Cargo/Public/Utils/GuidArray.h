// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GuidArray.generated.h"

USTRUCT()
struct FGuidArray
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FGuid> Guids;
};
