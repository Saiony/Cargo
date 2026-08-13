// Fill out your copyright notice in the Description page of Project Settings.


#include "DeveloperSettings/CargoSettings.h"

TObjectPtr<UMissionsDatabase> UCargoSettings::GetMissionsDatabase() const
{
	UMissionsDatabase* LoadedDatabase = MissionsDatabase.LoadSynchronous();
	
	if (!LoadedDatabase)
	{
		UE_LOG(LogTemp, Error, TEXT("MissionsDatabase soft reference failed to load. Asset may have been deleted or moved."));
		return nullptr;
	}

	return LoadedDatabase;
}
