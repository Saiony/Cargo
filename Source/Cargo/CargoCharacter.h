// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CargoPlayerController.h"
#include "Components/TimelineComponent.h"
#include "GameplayFramework/CargoPlayerState.h"
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTimelineComponent> RotateTimelineComp;
	
	UPROPERTY(EditAnywhere, Category="Cargo|Curves")
	UCurveFloat* Curve_RotateShipWeight;
	
	UPROPERTY(EditAnywhere, Category="Cargo|Curves")
	UCurveFloat* Curve_RotateShipSteering;
	
	UPROPERTY(EditAnywhere, Category="Cargo|Curves")
	UCurveFloat* Curve_RotateShipSteeringBack;

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
	float WeightImbalanceMultiplier_Movement = 250;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	float WeightImbalanceMultiplier_Roll = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FVector2D FRMinMax = FVector2D(-10.0f, 10.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FVector2D ShipRotationMovementMinMax = FVector2D(-10.0f, 15.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FVector2D ShipRotationMovementMinMax_HighSpeed = FVector2D(-20.0f, 20.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	float MovementRotationImbalanceMultiplier = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FVector2D ShipAngleMinMax = FVector2D(-70.0f, 70.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	float ReverseGearMultiplier = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Audio")
	TObjectPtr<USoundBase> MovementSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float MouseSensitivity = 0.8f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float MaxSpeedContainerFalloff = 0.5f;
	
	FDelegateHandle HasteCVarDelegateHandle;
	
	float FR = 0;	
	
	float OriginalMaxSpeed = -1;
	
	float OriginalAcceleration = -1;
	
	float LastKnockbackTime = -1.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float KnockbackCooldown = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float KnockbackStrength = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float KnockbackSpeed = 3.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float ShipInclinationMultiplier = 0.5f;
	
	FVector KnockbackVelocity;
	
	float BoatInitialRoll;
	float BoatTargetRoll;
	
	UPROPERTY()
	TObjectPtr<ACargoPlayerState> CargoPlayerState;

	void OnHasteCVarChanged(IConsoleVariable* ConsoleVariable);
	
	FOnTimelineFloat UpdateFunctionFloat;
 
	UFUNCTION()
	void UpdateTimelineComp(float Output);
	
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

	UFUNCTION()
	void OnCargoHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	void PopRandomContainerFromTop(const FVector& HitDir);
	
	UFUNCTION(Exec)
	void PopContainersFromZ(int32 Z);

	void RotateShip(float TargetAngle, UCurveFloat* Curve);

	UFUNCTION()
	void OnEditModeChanged(bool bEditMode);
public:
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);
	
	void AttachPlaceable(APlaceable* Placeable, FVector WorldPos);

	UFUNCTION()
	void OnShipBalanceChanged(float NewBalance);

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	
};

