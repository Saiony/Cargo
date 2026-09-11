// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission/MissionData.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Island/IslandDistanceRow.h"
#include "Misc/MessageDialog.h"

void UMissionData::CalculateSuggestion()
{
#if WITH_EDITOR
	// Load the distance table and the selected mission difficulty multiplier from CargoSettings.
	const UCargoSettings* Settings = GetDefault<UCargoSettings>();
	const UDataTable* Table = Settings->IslandDistanceTable.LoadSynchronous();
	const float* DifficultyMultiplier = Settings->MissionDifficultyMultipliers.Find(Difficulty);
	if (!OriginTag.IsValid() || !DestinationTag.IsValid() || !Table ||
		Table->GetRowStruct() != FIslandDistanceRow::StaticStruct() || !DifficultyMultiplier)
	{
		FMessageDialog::Open(EAppMsgType::Ok, INVTEXT("Set the mission origin/destination and configure the island distance table and difficulty multiplier in CargoSettings."));
		return;
	}

	// Find the distance between origin and destination, accepting the route in either direction.
	const FIslandDistanceRow* Route = nullptr;
	for (const auto& Entry : Table->GetRowMap())
	{
		const auto* Row = reinterpret_cast<const FIslandDistanceRow*>(Entry.Value);
		if ((Row->OriginIsland == OriginTag && Row->DestinationIsland == DestinationTag) ||
			(Row->OriginIsland == DestinationTag && Row->DestinationIsland == OriginTag))
		{
			Route = Row;
			break;
		}
	}
	if (!Route)
	{
		FMessageDialog::Open(EAppMsgType::Ok, INVTEXT("Route not found in the island distance table. Reward was not changed."));
		return;
	}

	// Sum the value of all container types required for this delivery.
	double CargoValue = 0.0;
	for (const FCargoRequirement& Requirement : CargoRequirements)
	{
		const auto* ContainerReference = Settings->ContainersMap.Find(Requirement.CargoType);
		const UContainerDA* Container = ContainerReference ? ContainerReference->LoadSynchronous() : nullptr;
		if (!Container || Container->BasePrice < 0 || Requirement.Quantity < 1)
		{
			FMessageDialog::Open(EAppMsgType::Ok, INVTEXT("A cargo requirement has a missing container, invalid price or invalid quantity. Reward was not changed."));
			return;
		}
		// Value of this cargo type = required quantity x unit price from its ContainerDA.
		CargoValue += static_cast<double>(Requirement.Quantity) * Container->BasePrice;
	}

	// Suggestion = total cargo value x distance stored in the table (without unit conversion)
	// x global reward multiplier x difficulty multiplier, both from CargoSettings.
	const double Suggestion = CargoValue * Route->Distance * Settings->MissionRewardMultiplier * *DifficultyMultiplier;
	// Reject invalid values or values outside Money's range before replacing the current reward.
	if (!FMath::IsFinite(Suggestion) || Suggestion < 0.0 || Suggestion > MAX_int32 ||
		Route->Distance < 0.0f || Settings->MissionRewardMultiplier < 0.0f || *DifficultyMultiplier < 0.0f)
	{
		FMessageDialog::Open(EAppMsgType::Ok, INVTEXT("Invalid distance/multiplier or reward exceeds the supported range. Reward was not changed."));
		return;
	}

	Modify();
	// Round the suggestion to the nearest integer; Money remains editable after calculation.
	Reward.Money = FMath::RoundToInt32(Suggestion);
	PostEditChange();
#endif
}

UMissionData::UMissionData()
{
	Id = FGuid::NewGuid();
}

void UMissionData::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	
	Id = FGuid::NewGuid();
}
