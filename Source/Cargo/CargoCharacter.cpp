// Copyright Epic Games, Inc. All Rights Reserved.

#include "CargoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Cargo.h"
#include "BuoyancyComponent.h"
#include "DUETween.h"
#include "Components/AudioComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "Grid/Placeable.h"

static TAutoConsoleVariable<bool> CVarBoostMovement(TEXT("Cargo.Haste"), false, TEXT("Increases boat speed"),ECVF_Default);


ACargoCharacter::ACargoCharacter()
{
	// Set size for collision capsule
	RootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMeshComp"));
	SetRootComponent(RootMeshComponent);	
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
	FloatingMovement->MaxSpeed = 600.f;
	FloatingMovement->Acceleration = 400.f;
	FloatingMovement->Deceleration = 800.f;
	
	HasteCVarDelegateHandle = CVarBoostMovement.AsVariable()->OnChangedDelegate().AddUObject(this, &ThisClass::OnHasteCVarChanged);

	BuoyancyComp = CreateDefaultSubobject<UBuoyancyComponent>("BuoyancyComp");
	
	GridComp = CreateDefaultSubobject<UGridComponent>(TEXT("GridComp"));
	GridComp->SetupAttachment(MeshComponent);
	
	MovementAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MovementAudioComp"));	
	
	RotateTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotateTimelineComp"));
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;		
}

void ACargoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACargoCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACargoCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACargoCharacter::Look);
	}
	else
	{
		UE_LOG(LogCargo, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACargoCharacter::Move(const FInputActionValue& Value)
{	
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	auto ContainersSideTilt = WeightImbalanceMultiplier * FR / 10000;
	auto ShipSpeedSideTilt = WeightImbalanceMultiplier * FloatingMovement->Velocity.Size() / 10000;

	// route the input
	DoMove(MovementVector.X + ContainersSideTilt, MovementVector.Y);
}

void ACargoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

bool ShouldResetRotation = false;

void ACargoCharacter::DoMove(float Right, float Forward)
{	
	if (GetController<ACargoPlayerController>()->bEditMode)
		return;
	
	// Movement relative to actor
	const FVector ForwardDirection = GetActorForwardVector();
	const FVector RightDirection   = GetActorRightVector();

	const bool IsMovingBack = Forward < 0.0f;
	Right *= IsMovingBack ? -1 : 1; 
	FloatingMovement->Acceleration = IsMovingBack ? OriginalAcceleration * ReverseGearMultiplier : OriginalAcceleration;
	
	AddMovementInput(ForwardDirection, Forward);
	

	// Rotation
	if (Right != 0.f)
	{
		const float DeltaTime = GetWorld()->GetDeltaSeconds();
		auto SpeedRotationIncrement = ShipInclinationMultiplier * FloatingMovement->Velocity.Size() / FloatingMovement->MaxSpeed;
		SpeedRotationIncrement *= Right > 0 ? SpeedRotationIncrement : -SpeedRotationIncrement;
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1,0.0f,FColor::White,FString::Printf(TEXT("Speed: %.2f"), FloatingMovement->Velocity.Size()));
			GEngine->AddOnScreenDebugMessage(2,0.0f,FColor::White,FString::Printf(TEXT("Speed Rotation Increment: %.10f"),SpeedRotationIncrement));
		}
	
		//rotate Yaw
		FRotator Delta(0.f, Right * RotationSpeed * DeltaTime /*+ SpeedRotationIncrement*/, 0.f);
		AddActorLocalRotation(Delta);
		
		//rotate Roll
		float FinalAngle = GetPlayerState<ACargoPlayerState>()->GetShipBalanceRotation() + SpeedRotationIncrement;
		
		if (FloatingMovement->Velocity.Size() > FloatingMovement->MaxSpeed * 0.5f)
		{
			FinalAngle += SpeedRotationIncrement;
			FinalAngle = FMath::Clamp(FinalAngle, ShipRotationMovementMinMax_HighSpeed.X, ShipRotationMovementMinMax_HighSpeed.Y);
		}
		else
		{			
			FinalAngle = FMath::Clamp(FinalAngle, ShipRotationMovementMinMax.X, ShipRotationMovementMinMax.Y);
		}
			
		GetPlayerState<ACargoPlayerState>()->SetShipBalanceRotation(FinalAngle);
		RotateShip(GetPlayerState<ACargoPlayerState>()->GetShipBalanceTotal(), Curve_RotateShipSteering);
		//RotateShipSteering(GetPlayerState<ACargoPlayerState>()->GetShipBalanceTotal());
		
		ShouldResetRotation = true;
	}
	else
	{
		if (ShouldResetRotation)
		{
			ShouldResetRotation = false;
			GetPlayerState<ACargoPlayerState>()->SetShipBalanceRotation(0);
			
			RotateShip(GetPlayerState<ACargoPlayerState>()->GetShipBalanceTotal(), Curve_RotateShipSteeringBack);
			//RotateShipSteeringBack(GetPlayerState<ACargoPlayerState>()->GetShipBalanceTotal());
		}			
	}
}

void ACargoCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw * MouseSensitivity);
		AddControllerPitchInput(Pitch * MouseSensitivity);
	}
}

void ACargoCharacter::AttachPlaceable(APlaceable* Placeable, FVector WorldPos)
{    
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	Placeable->AttachToActor(this, AttachmentRules);
}

void ACargoCharacter::OnPlaceableAdded(APlaceable* Placeable)
{
	GetPlayerState<ACargoPlayerState>()->AddWeight(Placeable->Weight);
	BalanceShip();
	UpdateSpeed();
}

void ACargoCharacter::OnPlaceableRemoved(APlaceable* Placeable)
{	
	GetPlayerState<ACargoPlayerState>()->RemoveWeight(Placeable->Weight);
	BalanceShip();
	UpdateSpeed();
}


void ACargoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateEngineSoundIntensity();
	
	if (!KnockbackVelocity.IsNearlyZero())
	{
		AddActorWorldOffset(KnockbackVelocity * DeltaSeconds, true);
		KnockbackVelocity = FMath::VInterpTo(KnockbackVelocity, FVector::ZeroVector, DeltaSeconds, KnockbackSpeed);
	}
}

void ACargoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GridComp->OnPlaceableAddedToGrid.AddDynamic(this, &ThisClass::OnPlaceableAdded);
	GridComp->OnPlaceableRemovedFromGrid.AddDynamic(this, &ThisClass::OnPlaceableRemoved);	
	
	OriginalMaxSpeed = FloatingMovement->MaxSpeed;
	OriginalAcceleration = FloatingMovement->Acceleration;
	
	if (UPrimitiveComponent* CollisionComp = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		CollisionComp->SetNotifyRigidBodyCollision(true);
		CollisionComp->OnComponentHit.AddDynamic(this, &ACargoCharacter::OnCargoHit);
	}
	
	UpdateFunctionFloat.BindDynamic(this, &ACargoCharacter::UpdateTimelineComp);
	RotateTimelineComp->AddInterpFloat(Curve_RotateShipWeight, UpdateFunctionFloat, NAME_None, TEXT("Rotation"));
}

void ACargoCharacter::BalanceShip()
{
	FR = 0;
	for (const auto PlaceableKV : GridComp->GetOccupiedSlots())
	{
		const auto PlaceableWeight = PlaceableKV.Value->Weight;
		const auto Momentum = PlaceableWeight * PlaceableKV.Key.Y;
		
		FR += Momentum;
	}
	
	UE_LOG(LogTemp, Log, TEXT("FR: %f"), FR);
	
	const float FinalAngle = FMath::GetMappedRangeValueClamped(FRMinMax,ShipAngleMinMax, FR);	
	RotateShip(FinalAngle, Curve_RotateShipWeight);
	GetPlayerState<ACargoPlayerState>()->SetShipBalanceWeight(FinalAngle);
}

void ACargoCharacter::UpdateEngineSoundIntensity()
{
	const float SpeedValue = FloatingMovement->Velocity.Size2D();
	const float NormalizedSpeed = FMath::Clamp(SpeedValue / FloatingMovement->GetMaxSpeed(), 0.f, 1.f);

	MovementAudioComp->SetFloatParameter(FName("Speed"), NormalizedSpeed);
}

void ACargoCharacter::UpdateSpeed()
{
	FloatingMovement->MaxSpeed = OriginalMaxSpeed * GetPlayerState<ACargoPlayerState>()->GetShipSpeedMultiplier();
	FloatingMovement->Acceleration = OriginalAcceleration * GetPlayerState<ACargoPlayerState>()->GetShipSpeedMultiplier();
}

void ACargoCharacter::OnHasteCVarChanged(IConsoleVariable* ConsoleVariable)
{
	if (IsTemplate() || !GetWorld() || !GetWorld()->IsGameWorld())
	{
		return;
	}

	const bool Haste = CVarBoostMovement.GetValueOnGameThread();

	FloatingMovement->MaxSpeed = Haste ? OriginalMaxSpeed * 2.f : OriginalMaxSpeed;
	FloatingMovement->Acceleration = Haste ? OriginalAcceleration * 4.f : OriginalAcceleration;
}

void ACargoCharacter::OnCargoHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastKnockbackTime < KnockbackCooldown)
		return;
	
	LastKnockbackTime = Now;
	
	UE_LOG(LogTemp, Log, TEXT("Hit %s"), *OtherActor->GetName());
		
	const float HitVelocity = FloatingMovement->Velocity.Size();
	KnockbackVelocity = Hit.ImpactNormal.GetSafeNormal() * KnockbackStrength * 100.f;
	
	if (HitVelocity > OriginalMaxSpeed * 0.8f)
		PopRandomContainer();
}

void ACargoCharacter::PopRandomContainer()
{
	const auto HighestOccupiedZ = GridComp->GetHighestOccupiedZ();
	const auto PositionsTop = GridComp->GetPositionsFromLevel(HighestOccupiedZ);
	
	if (PositionsTop.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("No positions to pop"));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Popping random container"));
	
	const auto RandomIndex = FMath::RandRange(0, PositionsTop.Num() - 1);
	const auto RandomPosition = PositionsTop[RandomIndex];	
	
	auto Placeable = GridComp->GetPlaceableAt(RandomPosition);		
	GridComp->RemovePlaceableFromGrid(Placeable);
	
	const FVector RandomDirection = FMath::VRandCone(FVector::UpVector,FMath::DegreesToRadians(25.0f));
	Placeable->LaunchPlaceable(RandomDirection);
}

void ACargoCharacter::RotateShip(float TargetAngle, UCurveFloat* Curve)
{
	BoatInitialRoll = MeshComponent->GetRelativeRotation().Roll;
	BoatTargetRoll = TargetAngle;

	//RotateTimelineComp->SetPlayRate(1.0f / Duration);
	RotateTimelineComp->SetFloatCurve(Curve, TEXT("Rotation"));
	RotateTimelineComp->PlayFromStart();
}

void ACargoCharacter::UpdateTimelineComp(float Output)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Timeline Output: %f | Initial: %f | Target: %f"),
		Output,
		BoatInitialRoll,
		BoatTargetRoll
	);

	const float CurrentYaw = FMath::Lerp(BoatInitialRoll,BoatTargetRoll,Output);

	FRotator Rotation = MeshComponent->GetRelativeRotation();
	Rotation.Roll = CurrentYaw;

	MeshComponent->SetRelativeRotation(Rotation);
}