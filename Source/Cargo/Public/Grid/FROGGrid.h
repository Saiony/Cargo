// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


template <typename T>
class UFROGGrid
{
	TMap<FIntVector, T> OccupiedSlots;
	int32 CellSize;
	FIntVector GridOrigin;
	FIntVector GridSize;
	TSet<FIntVector> InvalidSlots;

public:
	UFROGGrid(const int32 CellSize, const FIntVector Origin, const FIntVector GridSize,
	          const TArray<FIntVector>& InvalidSlots) :
		OccupiedSlots(TMap<FIntVector, T>()),
		CellSize(CellSize),
		GridOrigin(Origin),
		GridSize(GridSize),
		InvalidSlots(InvalidSlots)
	{
	}

	UFROGGrid(const int32 CellSize, const FIntVector Origin, const FIntVector GridSize) : UFROGGrid(
		CellSize, Origin, GridSize, TArray<FIntVector>())
	{
	}

	const T* GetValue(const int32 X, const int32 Y, const int32 Z) const
	{
		FIntVector Cell(X, Y, Z);
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

	void Add(const FIntVector& GridIndex, const T Value)
	{
		if (!IsSlotAvailable(GridIndex))
			return;

		FIntVector Cell(GridIndex);
		OccupiedSlots.Add(Cell, Value);
	}

	void Remove(const int32 X, const int32 Y, const int32 Z)
	{
		FIntVector Cell(X, Y, Z);

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

	TMap<FIntVector, T> GetOccupiedSlots() const
	{
		return OccupiedSlots;
	}
	
	bool IsInvalidSlot(const FIntVector& GridIndex) const
	{
		return InvalidSlots.Contains(GridIndex);
	}
	
	bool IsSlotAvailable(const FIntVector& GridIndex) const
	{
		const auto IsInvalid = IsInvalidSlot(GridIndex);
		const auto IsOccupied = OccupiedSlots.Contains(GridIndex);
		const auto WithinBounds = IsWithinBounds(GridIndex);

		return !IsInvalid && !IsOccupied && WithinBounds;
	}

	bool IsWithinBounds(const FIntVector& GridIndex) const
	{
		const FIntVector Min = GetMin();
		const FIntVector Max = GetMax();

		return GridIndex.X >= Min.X && GridIndex.X <= Max.X &&
			GridIndex.Y >= Min.Y && GridIndex.Y <= Max.Y;
	}

	FVector GetRoundedLocation(const FVector& WorldLocation) const
	{
		return FVector
		(
			GridOrigin.X + FMath::RoundToInt((WorldLocation.X - GridOrigin.X) / CellSize) * CellSize,
			GridOrigin.Y + FMath::RoundToInt((WorldLocation.Y - GridOrigin.Y) / CellSize) * CellSize,
			GridOrigin.Z + FMath::RoundToInt((WorldLocation.Z - GridOrigin.Z) / CellSize) * CellSize
		);
	}

	FIntVector GetMin() const
	{
		return FIntVector(-GridSize.X / 2, -GridSize.Y / 2, 0);
	}

	FIntVector GetMax() const
	{
		return FIntVector((GridSize.X - 1) / 2, (GridSize.Y -1) / 2, GridSize.Z);
	}

	friend bool operator!=(const UFROGGrid& lhs, const UFROGGrid& rhs)
	{
		return !(lhs == rhs);
	}

	FIntVector LocalToGrid(const FVector& LocalPosition) const
	{
		return LocalToGrid(LocalPosition.X, LocalPosition.Y, LocalPosition.Z);
	}

	FIntVector LocalToGrid(const float LocalX, const float LocalY, const float LocalZ) const
	{
		return FIntVector
		(
			FMath::FloorToInt((LocalX - GridOrigin.X) / CellSize),
			FMath::FloorToInt((LocalY - GridOrigin.Y) / CellSize),
			FMath::FloorToInt((LocalZ - GridOrigin.Z) / CellSize)
		);
	}

	FVector GridToLocal(const FIntVector& GridIndex) const
	{
		return FVector(
			GridOrigin.X + GridIndex.X * CellSize,
			GridOrigin.Y + GridIndex.Y * CellSize,
			0.0f
		);
	}

	int32 GetHighestOccupiedZ() const
	{
		if (OccupiedSlots.IsEmpty())
		{
			return 0;
		}

		int32 HighestZ = TNumericLimits<int32>::Lowest();

		for (const auto& Elem : OccupiedSlots)
		{
			HighestZ = FMath::Max(HighestZ, Elem.Key.Z);
		}

		return HighestZ;
	}

	TArray<FIntVector> GetOccupiedPositionsAtZ(int32 Z) const
	{
		TArray<FIntVector> Positions;

		int32 HighestZ = TNumericLimits<int32>::Lowest();

		for (const auto& Elem : OccupiedSlots)
		{
			if (Elem.Key.Z != Z)
				continue;

			Positions.Add(Elem.Key);
		}

		return Positions;
	}

	TSet<FIntVector> GetAllAvailableSlots()
	{
		const FIntVector Min = GetMin();
		const FIntVector Max = GetMax();

		TSet<FIntVector> AllCells;
		AllCells.Reserve((Max.X - Min.X + 1) * (Max.Y - Min.Y + 1));

		for (int32 Y = Min.Y; Y <= Max.Y; ++Y)
		{
			for (int32 X = Min.X; X <= Max.X; ++X)
			{
				AllCells.Add(FIntVector(X, Y, 0));
			}
		}

		TSet<FIntVector> OccupiedKeys;
		OccupiedSlots.GetKeys(OccupiedKeys);

		return AllCells.Difference(InvalidSlots).Difference(OccupiedKeys);
	}
};
