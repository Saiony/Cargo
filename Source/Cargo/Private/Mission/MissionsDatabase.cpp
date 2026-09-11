// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission/MissionsDatabase.h"

void UMissionsDatabase::ApplyLocationsToMissionOrigins()
{
#if WITH_EDITOR
	for (const auto& Entry : Database)
	{
		for (UMissionData* Mission : Entry.Value.Missions)
		{
			if (!IsValid(Mission) || Mission->OriginTag == Entry.Key)
			{
				continue;
			}

			Mission->Modify();
			Mission->OriginTag = Entry.Key;
			Mission->PostEditChange();
		}
	}
#endif
}

FMissions* UMissionsDatabase::GetMissionsForLocation(const FGameplayTag LocationTag)
{
	return Database.Find(LocationTag);
}
