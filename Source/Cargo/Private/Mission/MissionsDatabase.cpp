// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission/MissionsDatabase.h"

FMissions* UMissionsDatabase::GetMissionsForLocation(const FGameplayTag LocationTag)
{
	return Database.Find(LocationTag);
}
