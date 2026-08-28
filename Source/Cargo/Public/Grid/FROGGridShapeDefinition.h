// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "FROGGridShapeDefinition.generated.h"

/**
 * 
 */
USTRUCT()
struct CARGO_API FFROGGridShapeDefinition
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere)
	TArray<FIntVector> Cells;
	
	TArray<FIntVector> GetRotatedCells(float RotationDegrees) const
	{
		TArray<FIntVector> Result;
		Result.Reserve(Cells.Num());

		const int32 Steps = FMath::RoundToInt(RotationDegrees / 90.f) & 3; // 0..3

		for (const FIntVector& Cell : Cells)
		{
			FIntVector Rotated = Cell;
			for (int32 i = 0; i < Steps; ++i)
			{
				Rotated = FIntVector(-Rotated.Y, Rotated.X, Rotated.Z);
			}
			Result.Add(Rotated);
		}
		return Result;
	}
};
