// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


template <typename T>
class UFROGGrid 
{
public:
	UFROGGrid(const int32 CellSize, const FIntPoint Origin) :
		OccupiedSlots(TMap<FIntPoint, T>()),
		CellSize(CellSize),
		GridOrigin(Origin)
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
	
	FIntPoint WorldToGrid(const FVector& PositionXY) const
	{
		return WorldToGrid(PositionXY.X, PositionXY.Y);
	}

	FIntPoint WorldToGrid(const float WorldX, const float WorldY) const
	{
		return FIntPoint
		(
			FMath::FloorToInt((WorldX - GridOrigin.X) / CellSize),
			FMath::FloorToInt((WorldY - GridOrigin.Y) / CellSize)
		);
	}

	FVector GridToWorld(const FIntPoint& GridIndex) const
	{
		return FVector(
			GridOrigin.X + GridIndex.X * CellSize,
			GridOrigin.Y + GridIndex.Y * CellSize,
			0.0f
		);
	}

	friend bool operator!=(const UFROGGrid& lhs, const UFROGGrid& rhs)
	{
		return !(lhs == rhs);
	}

private:
	TMap<FIntPoint, T> OccupiedSlots;
	int32 CellSize;
	FIntPoint GridOrigin;
};
