// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CargoPlayerController.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "Grid/GridComponent.h"
#include "Logging/LogMacros.h"
#include "CargoCharacter.generated.h"

class UAudioComponent;
class UCanvasRenderTarget2D;
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
DECLARE_MULTICAST_DELEGATE(FOnShipMovementChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeightChanged, float, NewCurrentWeight, float, MaxWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBalanceChanged, float, NewBalance);

/**
 *  A simple player-controllable third-person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ACargoCharacter : public APawn
{
	GENERATED_BODY()	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Weight")
	float CurrentWeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Weight")
	float MaxWeight = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo|Weight")
	float ShipSpeedMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Balance")
	float ShipBalanceWeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Balance")
	float ShipBalanceRotation = 0.f;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo|Audio")
	TObjectPtr<UAudioComponent> CollisionAudioComp;
	
	/*decal*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> ShipNameDecalComp;
	
	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> ShipNameRenderTarget;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ShipNameMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	TObjectPtr<UFont> ShipNameFont;	
	/*decal*/
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
#pragma region Rotation

	/** Yaw speed in degrees per second. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Yaw")
	float YawTurnSpeed = 120;
	
	/** Cargo imbalance influence on steering input. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Yaw")
	float CargoSteeringInfluence_StraightLine = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Yaw")
	float CargoSteeringInfluence_PlayerRotating = 2;

	/** Scaled by speed and steering. */
	UPROPERTY(EditAnywhere, Category="Cargo|Rotation|Roll|Movement")
	float MovementRollSensitivity = 10.0f;

	/** Extra movement roll buildup, zero disables the load influence. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Roll|Movement", meta=(ClampMin="0.0"))
	float LoadRollSensitivity = 1.f;

	/** Movement roll limits in degrees at up to half speed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Roll|Movement")
	FVector2D MovementRollLimits = FVector2D(-10.0f, 10.0f);
	
	/** Movement roll limits in degrees above half speed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Roll|Movement")
	FVector2D HighSpeedMovementRollLimits = FVector2D(-20.0f, 20.0f);

	/** Roll degrees per weight unit one cell from the centerline. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Roll|Cargo")
	float CargoRollSensitivity = 0.5f;

	/** Cargo-only roll limits in degrees. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Roll|Cargo")
	FVector2D CargoRollLimits = FVector2D(-70.0f, 70.0f);

	/** Roll interpolation speed; zero snaps to the target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Roll", meta=(ClampMin="0.0"))
	float RollResponseSpeed = 4.f;

	/** Roll interpolation speed while the steering roll resets after input stops. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Roll", meta=(ClampMin="0.0"))
	float ResetRollResponseSpeed = 1.5f;

	/** Mouse look sensitivity for camera yaw and pitch. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Rotation|Camera")
	float CameraLookSensitivity = 0.8f;

	/** Total cargo weight times lateral distance in grid cells. */
	float CargoWeightMoment = 0;
	/** Combined movement and cargo roll target in degrees. */
	float TargetRoll = 0.f;
	/** Whether releasing steering must clear movement roll. */
	bool bNeedsMovementRollReset = false;
	/** Whether the current roll target is being reached via steering recovery. */
	bool bIsReturningMovementRoll = false;

#pragma endregion Rotation
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	float ReverseGearMultiplier = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Audio")
	TObjectPtr<USoundBase> MovementSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float MaxSpeedContainerFalloff = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float FuelConsumptionPerTick = 1;
	
	FDelegateHandle HasteCVarDelegateHandle;
	
	float OriginalMaxSpeed = -1;
	
	float OriginalAcceleration = -1;
	
	float LastKnockbackTime = -1.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float KnockbackCooldown = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float KnockbackStrength = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float KnockbackSpeed = 3.f;

	/** Shake displacement per unit of impact speed. */
	UPROPERTY(EditDefaultsOnly, Category="Cargo|Shake", meta=(ClampMin="0.0"))
	float ContainerShakeIntensity = 0.04f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo|Shake", meta=(ClampMin="0.0"))
	float ContainerShakeDuration = 0.3f;
	
	FVector KnockbackVelocity;

	UPROPERTY()
	TObjectPtr<ACargoPlayerState> CargoPlayerState;

	void OnHasteCVarChanged(IConsoleVariable* ConsoleVariable);
	
public:
	UPROPERTY(BlueprintAssignable, Category="Cargo|Weight")
	FOnWeightChanged OnWeightChanged;

	UPROPERTY(BlueprintAssignable, Category="Cargo|Balance")
	FOnBalanceChanged OnBalanceChanged;

	UFUNCTION(BlueprintPure, Category="Cargo|Weight")
	float GetCurrentWeight() const { return CurrentWeight; }

	UFUNCTION(BlueprintPure, Category="Cargo|Weight")
	float GetMaxWeight() const { return MaxWeight; }

	UFUNCTION(BlueprintPure, Category="Cargo|Weight")
	float GetShipSpeedMultiplier() const { return ShipSpeedMultiplier; }

	UFUNCTION(BlueprintPure, Category="Cargo|Balance")
	float GetShipBalanceTotal() const { return ShipBalanceWeight + ShipBalanceRotation; }

	UFUNCTION(BlueprintPure, Category="Cargo|Balance")
	float GetShipBalanceWeight() const { return ShipBalanceWeight; }

	UFUNCTION(BlueprintPure, Category="Cargo|Balance")
	float GetShipBalanceRotation() const { return ShipBalanceRotation; }

	UFUNCTION(BlueprintPure, Category="Cargo|Balance")
	float GetCurrentShipRoll() const;

	UFUNCTION(Category="Cargo|Weight")
	void AddWeight(float Weight);

	UFUNCTION(Category="Cargo|Weight")
	void RemoveWeight(float Weight);

	UFUNCTION(Category="Cargo|Weight")
	void SetMaxWeight(float NewMaxWeight);

	UFUNCTION(Category="Cargo|Balance")
	void SetShipBalanceWeight(float NewBalance);

	UFUNCTION(Category="Cargo|Balance")
	void SetShipBalanceRotation(float NewBalance);

	/** Constructor */
	ACargoCharacter();	

	FOnShipMovementChanged OnMovementStarted;
	FOnShipMovementChanged OnMovementStopped;
	bool IsShipMoving() const { return bShipMoving; }

protected:
	bool bShipMoving = false;
	void UpdateMovementState(float Forward);
	void StopMovementInput();

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

	void OnFuelDepleted();
	
	UFUNCTION()
	void OnPlaceableAdded(APlaceable* Placeable);
	
	UFUNCTION()
	void OnPlaceableRemoved(APlaceable* Placeable);

	UFUNCTION()
	void OnShipHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	void PopRandomContainerFromTop(const FVector& HitDir);
	
	UFUNCTION(Exec)
	void PopContainersFromZ(int32 Z);

	void RotateShip(float TargetAngle);

	UFUNCTION()
	void OnEditModeChanged(bool bEditMode);

	UFUNCTION()
	void DrawShipName(UCanvas* Canvas, int Width, int Height);
	
	void InitializeShipName();
	void CalculateShipSpeedMultiplier();
public:
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);
	
	void AttachPlaceable(APlaceable* Placeable, FVector WorldPos);

	UFUNCTION()
	void OnShipNameChanged(FString NewShipName);
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
};

