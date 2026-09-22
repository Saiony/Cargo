// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ContainerDA.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "Island/IslandData.h"
#include "Mission/MissionsDatabase.h"
#include "UI/MapWidget.h"
#include "UI/Generic/CargoInputTextWidget.h"
#include "CargoSettings.generated.h"

class AContainer;
class APlaceablePreview;
class UBOLWidget;
class USimplePrompt;

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
	UPROPERTY(EditAnywhere, Config, Category = "Mission", meta = (RequiredAssetDataTags = "RowStructure=/Script/Cargo.IslandDistanceRow"))
	TSoftObjectPtr<UDataTable> IslandDistanceTable;

	UPROPERTY(EditAnywhere, Config, Category = "Mission", meta = (ClampMin = "0"))
	float MissionRewardMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Config, Category = "Mission", meta = (ClampMin = "0"))
	TMap<EMissionDifficulty, float> MissionDifficultyMultipliers = {
		{EMissionDifficulty::Easy, 1.0f},
		{EMissionDifficulty::Medium, 1.5f},
		{EMissionDifficulty::Hard, 2.0f}
	};

	UPROPERTY(EditAnywhere, Config, Category = "Data Assets", meta = (Categories = "Cargo"))
	TMap<FGameplayTag, TSoftObjectPtr<UContainerDA>> ContainersMap;
	
	UPROPERTY(EditAnywhere, Config, Category = "Grid")
	float GridCellSize;
	
	UPROPERTY(EditAnywhere, Config, Category = "Container")
	TSoftClassPtr<APlaceablePreview> PlaceablePreviewClass;
	
	UPROPERTY(EditAnywhere, Config, Category = "UI")
	TSubclassOf<UMapWidget> MapWidgetClass;

	UPROPERTY(EditAnywhere, Config, Category = "UI")
	TSoftClassPtr<UBOLWidget> BOLWidgetClass;
	
	UPROPERTY(EditAnywhere, Config, Category = "UI")
	TSoftClassPtr<UCargoInputTextWidget> InputTextWidgetClass;

	UPROPERTY(EditAnywhere, Config, Category = "UI")
	TSoftClassPtr<USimplePrompt> FuelDepletedPromptClass;

	UPROPERTY(EditAnywhere, Config, Category = "UI|Dialogue")
	FLinearColor DialoguePlayerDataColor = FLinearColor(1.0f, 0.65f, 0.1f, 1.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo|Island")
	TSubclassOf<AContainer> ContainerClass;	
	
	UPROPERTY(EditAnywhere, Config, Category = "Cargo|Island", meta = (Categories = "Location"))
	TMap<FGameplayTag, TSoftObjectPtr<UIslandData>> IslandsMap;
	
	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	TSoftObjectPtr<UStaticMesh> SimpleCube;
	
	TObjectPtr<UMissionsDatabase> GetMissionsDatabase() const;
	
	virtual FName GetCategoryName() const override { return FName("Cargo"); }
};
