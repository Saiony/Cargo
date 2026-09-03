#include "Mission/MissionReward.h"

#include "Mission/MissionStatus.h"

FMissionReward::FMissionReward(TObjectPtr<UMissionStatus> MissionStatus) : BaseReward(MissionStatus->GetBaseReward())
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

FReward FMissionReward::CalculateFinalReward(TObjectPtr<UMissionStatus> MissionStatus) const
{
	FReward Reward;

	Reward.Money = MissionStatus->GetBaseReward().Money;
	Reward.Money -= MissingCargoDiscount;
	Reward.Money -= DamagedCargoDiscount;
	Reward.Money -= RecklessNavigationDiscount;

	return Reward;
}

int8 FMissionReward::CalculateStars(TObjectPtr<UMissionStatus> MissionStatus, const FReward& Reward) const
{
	const int32 Percentage = Reward.Money / MissionStatus->GetBaseReward().Money;

	if (Percentage < 30)
		return 1;
	if (Percentage < 80)
		return 2;

	return 3;
}

int32 FMissionReward::CalculateDamagedCargoDiscount(TObjectPtr<UMissionStatus> MissionStatus) const
{
	int32 Discount = 0;

	for (const auto Delivered : MissionStatus->GetDeliveredQuantities())
	{
		if (Delivered.Value.IsDamaged)
			Discount += Delivered.Value.BasePrice * 0.5f;
	}

	return Discount;
}

int32 FMissionReward::CalculateMissingCargoDiscount(TObjectPtr<UMissionStatus> MissionStatus) const
{
	int32 Discount = 0;

	for (const auto Delivered : MissionStatus->GetDeliveredQuantities())
	{
		const auto MissingCargoCount = Delivered.Value.TotalQuantity - Delivered.Value.DeliveredQuantity;
		Discount += MissingCargoCount * Delivered.Value.BasePrice * 0.5f;
	}

	return Discount;
}

int32 FMissionReward::CalculateRecklessNavigationDiscount(TObjectPtr<UMissionStatus> MissionStatus) const
{
	const int32 LightCollisionDiscount = MissionStatus->GetNumShipCollisions_Light() * MissionStatus->GetBaseReward().Money * 0.02f;
	const int32 HardCollisionDiscount = MissionStatus->GetNumShipCollisions_Hard() * MissionStatus->GetBaseReward().Money * 0.05f;

	return LightCollisionDiscount + HardCollisionDiscount;
}
