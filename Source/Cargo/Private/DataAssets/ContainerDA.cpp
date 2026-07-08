// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/ContainerDA.h"

TArray<FVector> UContainerDA::GetAllGridPositions(const FVector& BaseLocation, const float Rotation, const float CellSize) const
{
	TArray<FVector> Locations;

	for (int i = 0; i < Size.X; i++)
	{
		for (int j = 0; j < Size.Y; j++)
		{
			const auto OffsetX = i * CellSize;
			const auto OffsetY = j * CellSize;

			float RotatedX, RotatedY;

			if (FMath::IsNearlyEqual(Rotation, 0.0f)) // 0 degrees
			{
				RotatedX = OffsetX;
				RotatedY = OffsetY;
			}
			else if (FMath::IsNearlyEqual(Rotation, 90.0f)) // 90 degrees
			{
				RotatedX = -OffsetY;
				RotatedY = OffsetX;
			}
			else if (FMath::IsNearlyEqual(Rotation, 180.0f)) // 180 degrees
			{
				RotatedX = -OffsetX;
				RotatedY = -OffsetY;
			}
			else if (FMath::IsNearlyEqual(Rotation, -90.0f)) // 270 degrees
			{
				RotatedX = OffsetY;
				RotatedY = -OffsetX;
			}
			else
			{
				RotatedX = OffsetX;
				RotatedY = OffsetY;
			}

			Locations.Add(FVector(BaseLocation.X + RotatedX, BaseLocation.Y + RotatedY, 0.0f));
		}
	}

	return Locations;
}