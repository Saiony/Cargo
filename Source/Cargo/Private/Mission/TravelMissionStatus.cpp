#include "Mission/TravelMissionStatus.h"

#include "Mission/TravelMissionData.h"

void UTravelMissionStatus::Initialize(const UTravelMissionData* MissionData)
{
	check(MissionData);
	DestinationTag = MissionData->DestinationTag;
}
