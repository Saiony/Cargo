#include "MissionStatusFactory.h"

#include "Mission/DeliveryMissionData.h"
#include "Mission/DeliveryMissionStatus.h"
#include "Mission/TravelMissionData.h"
#include "Mission/TravelMissionStatus.h"

UBaseMissionStatus* MissionStatusFactory::CreateMissionStatus(UObject* Outer, UBaseMissionData* Data)
{
	if (const auto DeliveryMissionData = Cast<UDeliveryMissionData>(Data))
	{
		const auto Status = NewObject<UDeliveryMissionStatus>(Outer);
		Status->Initialize(DeliveryMissionData, DeliveryMissionData->OriginTag, FGuid::NewGuid());
		return Status;
	}

	if (const auto* TravelMissionData = Cast<UTravelMissionData>(Data))
	{
		auto* Status = NewObject<UTravelMissionStatus>(Outer);
		Status->Initialize(TravelMissionData);
		return Status;
	}

	return nullptr;
}
