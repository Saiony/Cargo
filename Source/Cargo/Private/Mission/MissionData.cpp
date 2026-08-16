// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission/MissionData.h"

UMissionData::UMissionData()
{
	Id = FGuid::NewGuid();
}

void UMissionData::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	
	Id = FGuid::NewGuid();
}
