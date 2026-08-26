// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ContainerDA.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "Mission/MissionsDatabase.h"
#include "UI/MapWidget.h"
#include "CargoSettings.generated.h"

class AContainer;
class APlaceablePreview;

struct FGameplayTag;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game Settings"))
class CARGO_API UCargoSettings : public UDeveloperSettings
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, Config, Category = "Mission")
	TSoftObjectPtr<UMissionsDatabase> MissionsDatabase;
	
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "Data Assets")
	TMap<FGameplayTag, TSoftObjectPtr<UContainerDA>> ContainersMap;
	
	UPROPERTY(EditAnywhere, Config, Category = "Grid")
	float GridCellSize;
	
	UPROPERTY(EditAnywhere, Config, Category = "Container")
	TSoftClassPtr<APlaceablePreview> PlaceablePreviewClass;
	
	UPROPERTY(EditAnywhere, Config, Category = "UI")
	TSubclassOf<UMapWidget> MapWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo|Island")
	TSubclassOf<AContainer> ContainerClass;
	
	TObjectPtr<UMissionsDatabase> GetMissionsDatabase() const;
	
	virtual FName GetCategoryName() const override { return FName("Cargo"); }
};
