// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/LevelDatabase.h"

#include "Subsystem/LevelManagerSubsystem.h"

FLevelData ULevelDatabase::GetLevelData(ELevelType LevelType) const
{
	const auto Level = Levels.Find(LevelType);	
	check(Level);
	
	return *Level;
}
