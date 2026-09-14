#pragma once
#include "Mission/BaseMissionData.h"

class UBaseMissionStatus;

class MissionStatusFactory
{
public:
	static UBaseMissionStatus* CreateMissionStatus(UObject* Outer, UBaseMissionData* Data);
};
