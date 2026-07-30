// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CargoPlayerController.h"
#include "Grid/GridComponent.h"
#include "Logging/LogMacros.h"
#include "CargoCharacter.generated.h"

class UGameplayCameraComponent;
class UBuoyancyComponent;
class USphereComponent;
class UFloatingPawnMovement;
class APlaceable;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third-person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ACargoCharacter : public APawn
{
	GENERATED_BODY()	
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RootMeshComponent; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* FloatingMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBuoyancyComponent* BuoyancyComp; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UGridComponent> GridComp;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAudioComponent> MovementAudioComp;	

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	float WeightInbalanceMultiplier = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FVector2D FRMinMax = FVector2D(-10.0f, 10.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FVector2D ShipAngleMinMax = FVector2D(-45.0f, 45.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Audio")
	TObjectPtr<USoundBase> MovementSound;
	
	FDelegateHandle HasteCVarDelegateHandle;
	
	float FR = 0;	
	
	float OriginalMaxSpeed = -1;
	
	float OriginalAcceleration = -1;
	
	void OnHasteCVarChanged(IConsoleVariable* ConsoleVariable);
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
	
	void MoveForward(const FInputActionValue& InputActionValue);
	
	void BalanceShip();
	
	void UpdateEngineSoundIntensity();

	void UpdateSpeed();
	UFUNCTION()
	void OnPlaceableAdded(APlaceable* Placeable);
	
	UFUNCTION()
	void OnPlaceableRemoved(APlaceable* Placeable);

public:
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);
	
	void AttachPlaceable(APlaceable* Placeable, FVector WorldPos);
	
	UFUNCTION(BlueprintImplementableEvent, Category="Cargo")
	void RotateShip(float FinalAngle);	
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
};

