// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


template <typename T>
class UFROGGrid 
{
	TMap<FIntPoint, T> OccupiedSlots;
	int32 CellSize;
	FIntPoint GridOrigin;
	
	FIntPoint GridSize;
	
public:
	UFROGGrid(const int32 CellSize, const FIntPoint Origin, const FIntPoint GridSize) :
		OccupiedSlots(TMap<FIntPoint, T>()),
		CellSize(CellSize),
		GridOrigin(Origin),
		GridSize(GridSize)
	{
	}

	const T* GetValue(const int32 X, const int32 Y) const
	{
		FIntPoint Cell(X, Y);
		return OccupiedSlots.Find(Cell);
	}

	TArray<T> GetValuesArray() const
	{
		TArray<T> Values;
		for (const auto& Elem : OccupiedSlots)
		{
			Values.Add(Elem.Value);
		}
		return Values;
	}

	void Add(const int32 X, const int32 Y, const T Value)
	{
		FIntPoint Cell(X, Y);
		OccupiedSlots.Add(Cell, Value);
	}

	void Remove(const int32 X, const int32 Y)
	{
		FIntPoint Cell(X, Y);
		
		OccupiedSlots.Remove(Cell);
	}

	void Clear()
	{
		OccupiedSlots.Empty();
	}

	int32 GetCellSize() const
	{
		return CellSize;
	}

	TMap<FIntPoint, T> GetOccupiedSlots() const
	{
		return OccupiedSlots;
	}

	FVector GetRoundedLocation(const FVector& WorldLocation) const
	{
		return FVector
		(
			GridOrigin.X + FMath::RoundToInt((WorldLocation.X - GridOrigin.X) / CellSize) * CellSize,
			GridOrigin.Y + FMath::RoundToInt((WorldLocation.Y - GridOrigin.Y) / CellSize) * CellSize,
			WorldLocation.Z
		);
	}
	
	FIntPoint GetMin() const
	{
		return FIntPoint(-GridSize.X / 2, -GridSize.Y / 2);
	}

	FIntPoint GetMax() const
	{
		return FIntPoint(GridSize.X / 2, GridSize.Y / 2);
	}

	friend bool operator!=(const UFROGGrid& lhs, const UFROGGrid& rhs)
	{
		return !(lhs == rhs);
	}
	
	FIntPoint LocalToGrid(const FVector& LocalPositionXY) const
	{
		return LocalToGrid(LocalPositionXY.X, LocalPositionXY.Y);
	}

	FIntPoint LocalToGrid(const float LocalX, const float LocalY) const
	{
		return FIntPoint
		(
		   FMath::FloorToInt((LocalX - GridOrigin.X) / CellSize),
		   FMath::FloorToInt((LocalY - GridOrigin.Y) / CellSize)
		);
	}

	FVector GridToLocal(const FIntPoint& GridIndex) const
	{
		return FVector(
		   GridOrigin.X + GridIndex.X * CellSize,
		   GridOrigin.Y + GridIndex.Y * CellSize,
		   0.0f
		);
	}
	
	bool IsWithinBounds(const FIntPoint& GridIndex) const
	{
		const FIntPoint Min = GetMin();
		const FIntPoint Max = GetMax();

		return GridIndex.X >= Min.X && GridIndex.X <= Max.X &&
			   GridIndex.Y >= Min.Y && GridIndex.Y <= Max.Y;
	}
};
