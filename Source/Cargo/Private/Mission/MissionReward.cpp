#include "Mission/MissionReward.h"

#include "DeveloperSettings/CargoSettings.h"
#include "Mission/DeliveryMissionStatus.h"

class UCargoSettings;

FMissionReward::FMissionReward(TObjectPtr<UDeliveryMissionStatus> MissionStatus) : BaseReward(MissionStatus->GetBaseReward())
{
	StartIslandTag = MissionStatus->GetStartIslandTag();
	DestinationTag = MissionStatus->GetDestinationTag();
	DeliveredQuantities = MissionStatus->GetDeliveredQuantities();

	MissingCargoDiscount = CalculateMissingCargoDiscount(MissionStatus);
	DamagedCargoDiscount = CalculateDamagedCargoDiscount(MissionStatus);
	RecklessNavigationDiscount = CalculateRecklessNavigationDiscount(MissionStatus);

	FinalReward = CalculateFinalReward(MissionStatus);
	Stars = CalculateStars(MissionStatus, FinalReward);
}

FReward FMissionReward::CalculateFinalReward(TObjectPtr<UDeliveryMissionStatus> MissionStatus) const
{
	FReward Reward;

	Reward.Money = MissionStatus->GetBaseReward().Money;
	Reward.Money -= MissingCargoDiscount;
	Reward.Money -= DamagedCargoDiscount;
	Reward.Money -= RecklessNavigationDiscount;

	return Reward;
}

int8 FMissionReward::CalculateStars(TObjectPtr<UDeliveryMissionStatus> MissionStatus, const FReward& Reward) const
{
	const float Percentage = static_cast<float>(Reward.Money) / static_cast<float>(MissionStatus->GetBaseReward().Money);

	if (Percentage < 0.3f) //30%
		return 1;
	if (Percentage < 0.8f) //80%
		return 2;

	return 3;
}

int32 FMissionReward::CalculateDamagedCargoDiscount(TObjectPtr<UDeliveryMissionStatus> MissionStatus) const
{
	int32 Discount = 0;

	for (const auto Delivered : MissionStatus->GetDeliveredQuantities())
	{
		if (!Delivered.Value.IsDamaged)
			continue;
		
		const auto BasePrice = GetDefault<UCargoSettings>()->ContainersMap.Find(Delivered.Value.CargoType)->Get()->BasePrice;
		Discount += BasePrice * 0.5f;
	}

	return Discount;
}

int32 FMissionReward::CalculateMissingCargoDiscount(TObjectPtr<UDeliveryMissionStatus> MissionStatus) const
{
	int32 Discount = 0;

	for (const auto Delivered : MissionStatus->GetDeliveredQuantities())
	{
		const auto BasePrice = GetDefault<UCargoSettings>()->ContainersMap.Find(Delivered.Value.CargoType)->Get()->BasePrice;

		const auto MissingCargoCount = Delivered.Value.TotalQuantity - Delivered.Value.DeliveredQuantity;
		Discount += MissingCargoCount * BasePrice;
	}

	return Discount;
}

int32 FMissionReward::CalculateRecklessNavigationDiscount(TObjectPtr<UDeliveryMissionStatus> MissionStatus) const
{
	const int32 LightCollisionDiscount = MissionStatus->GetNumShipCollisions_Light() * MissionStatus->GetBaseReward().Money * 0.02f;
	const int32 HardCollisionDiscount = MissionStatus->GetNumShipCollisions_Hard() * MissionStatus->GetBaseReward().Money * 0.05f;

	return LightCollisionDiscount + HardCollisionDiscount;
}
