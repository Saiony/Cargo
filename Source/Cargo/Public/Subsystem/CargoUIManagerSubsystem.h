// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "CargoUIManagerSubsystem.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS(Blueprintable)
class CARGO_API UCargoUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	UUserWidget* MapWidgetInstance;
	
public:
	void ToggleMap();
	
	void ShowMap();
	
	void HideMap();
	
	bool bIsMapVisible = false;
};
