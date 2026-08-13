// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/TextBlock.h"
#include "MissionRequirementEntryWidget.generated.h"

struct FCargoRequirement;
/**
 * 
 */
UCLASS()
class CARGO_API UMissionRequirementEntryWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> QuantityText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CommodityText;	
	
public:
	void Initialize(const FCargoRequirement& CargoRequirement);
};
