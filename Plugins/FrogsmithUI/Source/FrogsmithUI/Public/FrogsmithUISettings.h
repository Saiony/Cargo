// Copyright Frogsmith. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameUIPolicy.h"
#include "FrogsmithUISettings.generated.h"

/**
 * Settings for the FrogsmithUI plugin.
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Frogsmith UI"))
class FROGSMITHUI_API UFrogsmithUISettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFrogsmithUISettings() {}

	// The default UI policy class to use for the game.
	UPROPERTY(config, EditAnywhere, Category = "UI", meta = (AllowAbstract = "false"))
	TSoftClassPtr<UGameUIPolicy> DefaultUIPolicyClass;

public:
	//~UDeveloperSettings interface
	virtual FName GetCategoryName() const override { return FName("Frogsmith"); }
	//~End of UDeveloperSettings interface
};
