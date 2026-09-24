#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IslandOptionData.generated.h"

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class CARGO_API UIslandOptionData : public UObject
{
	GENERATED_BODY()
public:
	virtual FText GetButtonText() const PURE_VIRTUAL(UIslandOptionData::GetButtonText, return FText::GetEmpty(););
};
