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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	float YawRotationSpeed = 120;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	float WeightImbalanceMultiplier_Movement = 250;
	
	/** Degrees of cargo roll per unit of weight one grid cell from the centerline. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	float WeightImbalanceMultiplier_Roll = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FVector2D ShipRotationMovementMinMax = FVector2D(-10.0f, 10.0f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FVector2D ShipRotationMovementMinMax_HighSpeed = FVector2D(-20.0f, 20.0f);
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float FuelConsumptionPerTick = 1;
	
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

	/** Shake displacement per unit of impact speed. */
	UPROPERTY(EditDefaultsOnly, Category="Cargo|Shake", meta=(ClampMin="0.0"))
	float ContainerShakeIntensity = 0.04f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo|Shake", meta=(ClampMin="0.0"))
	float ContainerShakeDuration = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float ShipInclinationMultiplier = 0.5f;
	
	FVector KnockbackVelocity;
	
	/** How quickly the visual roll follows its target. Zero applies the target immediately. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Roll", meta=(ClampMin="0.0"))
	float RollInterpSpeed = 4.f;

	float BoatTargetRoll = 0.f;

	bool ShouldResetRotation = false;

	UPROPERTY()
	TObjectPtr<ACargoPlayerState> CargoPlayerState;

	void OnHasteCVarChanged(IConsoleVariable* ConsoleVariable);
	
public:
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

