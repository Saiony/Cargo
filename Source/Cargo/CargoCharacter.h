// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "Grid/FROGGrid.h"
#include "Logging/LogMacros.h"
#include "CargoCharacter.generated.h"

class UFloatingPawnMovement;
class APlaceable;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UINTERFACE(MinimalAPI, Blueprintable)
class UGridActorInterface : public UInterface { GENERATED_BODY() };

class IGridActorInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Notify")
	void OnPlaceableGrabbed(APlaceable* Placeable);
};

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ACargoCharacter : public APawn, public IGridActorInterface
{
	GENERATED_BODY()	
	
	// bool bEditMode = false;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* FloatingMovement;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	float RotationSpeed = 120;

public:

	/** Constructor */
	ACargoCharacter();	

protected:
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);	

	float GetGridCellSize() const { return 100.0f; }
	
	//Positions relative to the ship
	UFROGGrid<APlaceable*> PlaceableGrid = UFROGGrid<APlaceable*>(GetGridCellSize(), FIntPoint(0, 0));
	
	void MoveForward(const FInputActionValue& InputActionValue);
	
	void BalanceShip();

public:
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);
	
	void AddPlaceableToGrid(APlaceable* Placeable, FVector WorldPos);
	
	virtual void OnPlaceableGrabbed_Implementation(APlaceable* Placeable) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Cargo")
	void RotateShip(float FinalAngle);
};

