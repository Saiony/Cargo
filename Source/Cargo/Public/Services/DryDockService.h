// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FORGServiceBase.h"
#include "Island/CargoIsland.h"
#include "DryDockService.generated.h"

class UCameraRigAsset;
class UShipUpgradeCategoryDA;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UDryDockService : public UFORGServiceBase
{
	GENERATED_BODY()
	
	float FadeDuration = 2.0f;

public:
	UDryDockService();

protected:
	UPROPERTY()
	TObjectPtr<ACargoIsland> PreviousIsland;
	
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, Category="Dry Dock|Camera")
	TObjectPtr<UShipUpgradeCategoryDA> SelectedCategory;

	UFUNCTION(BlueprintCallable, Category="Dry Dock|Camera")
	void SetSelectedCategory(UShipUpgradeCategoryDA* Category);

	UFUNCTION(BlueprintPure, Category="Dry Dock|Camera")
	UCameraRigAsset* GetSelectedCameraRig() const;
	
	void GoToDryDock(TObjectPtr<ACargoIsland> From);
	void LeaveDryDock();
};
