// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CargoEditorSettings.generated.h"

class UStaticMesh;
class UMaterialInterface;

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Debug"))
class CARGOEDITOR_API UCargoEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "Debug Visualization")
	TSoftObjectPtr<UStaticMesh> DebugCellMesh;
 
	UPROPERTY(EditAnywhere, Config, Category = "Debug Visualization")
	TSoftObjectPtr<UMaterialInterface> DebugCellMaterial;
	
	virtual FName GetCategoryName() const override { return FName("Cargo"); }
};
