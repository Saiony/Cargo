// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogsmithActivatableWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Map/MapInterestPlaceWidget.h"
#include "MapWidget.generated.h"

class UCanvasPanel;
/**
 * 
 */
UCLASS()
class CARGO_API UMapWidget : public UFrogsmithActivatableWidget
{
	GENERATED_BODY()
    
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapImage;
    
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerImage;
    
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MapCanvas;
    
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TSubclassOf<UMapInterestPlaceWidget> MapInterestPlaceClass;
    
	UPROPERTY(VisibleAnywhere, Category = "Cargo")
	TArray<TObjectPtr<UMapInterestPlaceWidget>> MapInterestPlaceWidgets;
    
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TObjectPtr<UTexture2D> IslandIcon;
    
	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	FVector2D MapCaptureCenter = FVector2D(55000.f, 25000.f);

	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	float MapCaptureOrthoWidth = 150000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	float PlayerMarkerUpdateInterval = 0.1f;

	FTimerHandle PlayerMarkerTimerHandle;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void UpdateInterestPlaces();
	void UpdatePlayerMarker();

	void SetSlotAnchorFromWorldPosition(UCanvasPanelSlot* PanelSlot, const FVector& WorldLocation) const;
	FVector2D WorldToMapAnchor(const FVector& WorldLocation) const;

public:
	void SetupMap();
	void TeardownMap();
};