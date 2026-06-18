// Copyright Frogsmith. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFrogsmithUIModule : public IModuleInterface
{
public:
	FFrogsmithUIModule();
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
