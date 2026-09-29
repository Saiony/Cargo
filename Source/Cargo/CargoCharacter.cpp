// Copyright Epic Games, Inc. All Rights Reserved.

#include "CargoCharacter.h"
#include "CargoGameMode.h"
#include "DeveloperSettings/CargoSettings.h"
#include "Private/UI/Prompt/SimplePrompt.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Cargo.h"
#include "BuoyancyComponent.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "Grid/Placeable.h"
#include "Grid/Container.h"
#include "Subsystem/CargoTweenSubsystem.h"

static TAutoConsoleVariable<bool> CVarBoostMovement(TEXT("Cargo.Haste"), false, TEXT("Increases boat speed"),ECVF_Default);


ACargoCharacter::ACargoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	RootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMeshComp"));
	SetRootComponent(RootMeshComponent);	
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->ComponentTags.AddUnique(UCargoTweenSubsystem::ShakeTargetTag);

	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
	FloatingMovement->MaxSpeed = 600.f;
	FloatingMovement->Acceleration = 400.f;
	FloatingMovement->Deceleration = 800.f;
	
	HasteCVarDelegateHandle = CVarBoostMovement.AsVariable()->OnChangedDelegate().AddUObject(this, &ThisClass::OnHasteCVarChanged);

	BuoyancyComp = CreateDefaultSubobject<UBuoyancyComponent>("BuoyancyComp");
	
	GridComp = CreateDefaultSubobject<UGridComponent>(TEXT("GridComp"));
	GridComp->SetupAttachment(MeshComponent);
	GridComp->ContainerFallAngle = 30;
	
	MovementAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MovementAudioComp"));	

	CollisionAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("CollisionAudioComp"));
	CollisionAudioComp->SetupAttachment(RootComponent);
	CollisionAudioComp->bAutoActivate = false;
	
	ShipNameDecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("ShipNameDecalComp"));
	ShipNameDecalComp->SetupAttachment(MeshComponent);
		
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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACargoCharacter::StopMovementInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ACargoCharacter::StopMovementInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACargoCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACargoCharacter::Look);
	}
	else
	{
		UE_LOG(LogCargo, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACargoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACargoCharacter::Move(const FInputActionValue& Value)
{	
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ACargoCharacter::DoMove(float Right, float Forward)
{	
	if (GetController<ACargoPlayerController>()->bEditMode)
		return;
	
	// Movement relative to actor
	const FVector ForwardDirection = GetActorForwardVector();

	const bool IsMovingBack = Forward < 0.0f;
	Right *= IsMovingBack ? -1 : 1; 
	FloatingMovement->Acceleration = IsMovingBack ? OriginalAcceleration * ReverseGearMultiplier : OriginalAcceleration;	
	
	AddMovementInput(ForwardDirection, Forward);
	UpdateMovementState(Forward);
	
	//fuel
	if (ForwardDirection.Size() > 0.0f)
	{
		CargoPlayerState->GetFuelDomain()->RemoveFuel(FuelConsumptionPerTick);
	}
	
	//Special Yaw rotation based on cargo imbalance
	const auto ContainersSideTilt_StraightLine = CargoSteeringInfluence_StraightLine * CargoWeightMoment / 10000;
	const FRotator RotationIncrease(0.f, ContainersSideTilt_StraightLine, 0.f);
	AddActorLocalRotation(RotationIncrease);
	
	// Rotation
	if (Right == 0.f)
	{
		if (bNeedsMovementRollReset)
		{
			bNeedsMovementRollReset = false;
			SetShipBalanceRotation(0);
			bIsReturningMovementRoll = true;
		}			
	}
	else
	{				
		//rotate Yaw
		const FRotator Delta(0.f, Right * YawTurnSpeed * GetWorld()->GetDeltaSeconds(), 0.f);
		AddActorLocalRotation(Delta);	
		
		
		//rotate Roll	
		const auto ContainersSideTilt_PlayerRotating = CargoSteeringInfluence_PlayerRotating* CargoWeightMoment / 10000;
		Right += ContainersSideTilt_PlayerRotating;
		
		const float RollInputDirection = FMath::Clamp(Right, -1.f, 1.f);
		
		const float LoadRatio = CurrentWeight / MaxWeight;
		const float SpeedRation = FloatingMovement->Velocity.Size() / FloatingMovement->MaxSpeed;
		
		const auto SpeedRotationIncrement = RollInputDirection * GetWorld()->GetDeltaSeconds() * MovementRollSensitivity * SpeedRation;
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1,0.0f,FColor::White,FString::Printf(TEXT("Speed: %.2f"), FloatingMovement->Velocity.Size()));
			GEngine->AddOnScreenDebugMessage(2,0.0f,FColor::White,FString::Printf(TEXT("Speed Rotation Increment: %.10f"),SpeedRotationIncrement));
		}
			
		const float LoadRollMultiplier = (1.0f + LoadRatio) * LoadRollSensitivity;
		float FinalAngle = (ShipBalanceRotation + SpeedRotationIncrement) * LoadRollMultiplier;
		
		if (FloatingMovement->Velocity.Size() > FloatingMovement->MaxSpeed * 0.5f)
		{
			FinalAngle += SpeedRotationIncrement;
			FinalAngle = FMath::Clamp(FinalAngle, HighSpeedMovementRollLimits.X, HighSpeedMovementRollLimits.Y);
		}
		else
		{			
			FinalAngle = FMath::Clamp(FinalAngle, MovementRollLimits.X, MovementRollLimits.Y);
		}
			
		SetShipBalanceRotation(FinalAngle);
		bIsReturningMovementRoll = false;
		
		bNeedsMovementRollReset = true;
	}
}

void ACargoCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw * CameraLookSensitivity);
		AddControllerPitchInput(Pitch * CameraLookSensitivity);
	}
}

void ACargoCharacter::AttachPlaceable(APlaceable* Placeable, FVector WorldPos)
{    
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	Placeable->AttachToActor(this, AttachmentRules);
}

void ACargoCharacter::OnPlaceableAdded(APlaceable* Placeable)
{
	AddWeight(Placeable->Weight);
	BalanceShip();
	UpdateSpeed();
}

void ACargoCharacter::OnPlaceableRemoved(APlaceable* Placeable)
{	
	RemoveWeight(Placeable->Weight);
	BalanceShip();
	UpdateSpeed();
}

void ACargoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateEngineSoundIntensity();

	FRotator Rotation = MeshComponent->GetRelativeRotation();
	const float ActiveRollResponseSpeed = bIsReturningMovementRoll ? RollReturnResponseSpeed : RollResponseSpeed;
	Rotation.Roll = FMath::FInterpTo(Rotation.Roll, TargetRoll, DeltaSeconds, ActiveRollResponseSpeed);
	if (!FMath::IsNearlyEqual(Rotation.Roll, MeshComponent->GetRelativeRotation().Roll, 0.001f))
		OnBalanceChanged.Broadcast(Rotation.Roll);
	if (bIsReturningMovementRoll && FMath::IsNearlyEqual(Rotation.Roll, TargetRoll, 0.01f))
		bIsReturningMovementRoll = false;
	MeshComponent->SetRelativeRotation(Rotation);
	GridComp->UpdateStackLean(Rotation.Roll);
	
	if (!KnockbackVelocity.IsNearlyZero())
	{
		AddActorWorldOffset(KnockbackVelocity * DeltaSeconds, true);
		KnockbackVelocity = FMath::VInterpTo(KnockbackVelocity, FVector::ZeroVector, DeltaSeconds, KnockbackSpeed);
	}
}

void ACargoCharacter::StopMovementInput()
{
	UpdateMovementState(0.f);
	if (bNeedsMovementRollReset)
	{
		bNeedsMovementRollReset = false;
		SetShipBalanceRotation(0.f);
		bIsReturningMovementRoll = true;
	}
}

void ACargoCharacter::UpdateMovementState(float Forward)
{
	const bool bMovingNow = !FMath::IsNearlyZero(Forward);
	if (bMovingNow == bShipMoving)
		return;

	bShipMoving = bMovingNow;
	if (bShipMoving)
		OnMovementStarted.Broadcast();
	else
		OnMovementStopped.Broadcast();
}

void ACargoCharacter::CalculateShipSpeedMultiplier()
{
	const float WeightRatio = MaxWeight > 0.f ? CurrentWeight / MaxWeight : 0.f;
	if (WeightRatio >= 0.99f)
		ShipSpeedMultiplier = 0.25f;
	else if (WeightRatio >= 0.75f)
		ShipSpeedMultiplier = 0.5f;
	else if (WeightRatio >= 0.50f)
		ShipSpeedMultiplier = 0.75f;
	else if (WeightRatio >= 0.25f)
		ShipSpeedMultiplier = 0.8f;
	else
		ShipSpeedMultiplier = 1.f;
}

void ACargoCharacter::AddWeight(float Weight)
{
	CurrentWeight += Weight;
	CalculateShipSpeedMultiplier();
	OnWeightChanged.Broadcast(CurrentWeight, MaxWeight);
}

void ACargoCharacter::RemoveWeight(float Weight)
{
	CurrentWeight -= Weight;
	CalculateShipSpeedMultiplier();
	OnWeightChanged.Broadcast(CurrentWeight, MaxWeight);
}

void ACargoCharacter::SetMaxWeight(float NewMaxWeight)
{
	MaxWeight = NewMaxWeight;
	CalculateShipSpeedMultiplier();
	OnWeightChanged.Broadcast(CurrentWeight, MaxWeight);
	UpdateSpeed();
}

void ACargoCharacter::SetShipBalanceWeight(float NewBalance)
{
	ShipBalanceWeight = NewBalance;
	RotateShip(GetShipBalanceTotal());
}

void ACargoCharacter::SetShipBalanceRotation(float NewBalance)
{
	ShipBalanceRotation = NewBalance;
	RotateShip(GetShipBalanceTotal());
}

float ACargoCharacter::GetCurrentShipRoll() const
{
	return MeshComponent ? MeshComponent->GetRelativeRotation().Roll : 0.f;
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
		CollisionComp->OnComponentHit.AddDynamic(this, &ACargoCharacter::OnShipHit);
	}
	
	CargoPlayerState = GetPlayerState<ACargoPlayerState>();
	CargoPlayerState->GetFuelDomain()->OnFuelDepleted.AddUObject(
		this, &ThisClass::OnFuelDepleted);
	GridComp->AddTickPrerequisiteActor(this);
	TargetRoll = MeshComponent->GetRelativeRotation().Roll;
	
	//bind events
	GetController<ACargoPlayerController>()->OnEditModeChanged.AddDynamic(this, &ACargoCharacter::OnEditModeChanged);
	OnEditModeChanged(GetController<ACargoPlayerController>()->bEditMode);
	
	CargoPlayerState->OnShipNameChanged.AddDynamic(this, &ThisClass::OnShipNameChanged);
	
	InitializeShipName();
}

void ACargoCharacter::OnFuelDepleted()
{
	const auto PromptClass = GetDefault<UCargoSettings>()->FuelDepletedPromptClass.LoadSynchronous();
	const auto GameMode = ACargoGameMode::Get(this);
	const auto Prompt = GameMode->UIService->ShowWidget<USimplePrompt>(PromptClass);

	Prompt->Initialize(FText::FromString(TEXT("Combustivel acabou, uma unidade de abastecimento sera acionada")), FText::FromString(TEXT("OK")),
	[GameMode, this]()
			{
				GameMode->UIService->FadeIn(1.f, [GameMode, this]()
				{
					const auto FuelDomain = CargoPlayerState->GetFuelDomain();
					FuelDomain->AddFuel(FuelDomain->GetMaxFuel());
			
					GameMode->EconomyService->IncrementFuelDebt();
					
					GameMode->UIService->FadeOut(3.f, []() {});
				});		
			});
}

void ACargoCharacter::BalanceShip()
{
	CargoWeightMoment = 0;
	for (const auto PlaceableKV : GridComp->GetOccupiedSlots())
	{
		//we only care about horizontal momentum
		const auto Momentum = PlaceableKV.Value->GetWeightPerCell() * PlaceableKV.Key.Y;
		
		CargoWeightMoment += Momentum;
	}
	
	UE_LOG(LogTemp, Log, TEXT("CargoWeightMoment: %f"), CargoWeightMoment);
	
	const float FinalAngle = FMath::Clamp(CargoWeightMoment * CargoRollSensitivity, CargoRollLimits.X, CargoRollLimits.Y);
	SetShipBalanceWeight(FinalAngle);
}

void ACargoCharacter::UpdateEngineSoundIntensity()
{
	const float SpeedValue = FloatingMovement->Velocity.Size2D();
	const float NormalizedSpeed = FMath::Clamp(SpeedValue / FloatingMovement->GetMaxSpeed(), 0.f, 1.f);

	MovementAudioComp->SetFloatParameter(FName("Speed"), NormalizedSpeed);
}

void ACargoCharacter::UpdateSpeed()
{
	FloatingMovement->MaxSpeed = OriginalMaxSpeed * ShipSpeedMultiplier;
	FloatingMovement->Acceleration = OriginalAcceleration * ShipSpeedMultiplier;
}

void ACargoCharacter::OnHasteCVarChanged(IConsoleVariable* ConsoleVariable)
{
	if (IsTemplate() || !GetWorld() || !GetWorld()->IsGameWorld())
	{
		return;
	}

	const bool Haste = CVarBoostMovement.GetValueOnGameThread();

	//FloatingMovement->MaxSpeed = Haste ? OriginalMaxSpeed * 2.f : OriginalMaxSpeed;
	FloatingMovement->Acceleration = Haste ? OriginalAcceleration * 10 : OriginalAcceleration;
}

void ACargoCharacter::OnShipHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastKnockbackTime < KnockbackCooldown)
		return;
	
	LastKnockbackTime = Now;
		
	const float HitVelocity = FloatingMovement->Velocity.Size();
	const float ShakeIntensity = HitVelocity * ContainerShakeIntensity;
	
	GetWorld()->GetSubsystem<UCargoTweenSubsystem>()->DoShake(this, ShakeIntensity, ContainerShakeDuration);
	
	TSet<AContainer*> ShakenContainers;
	for (const auto& Slot : GridComp->GetOccupiedSlots())
	{
		AContainer* Container = Cast<AContainer>(Slot.Value);
		if (IsValid(Container) && !ShakenContainers.Contains(Container))
		{
			ShakenContainers.Add(Container);
			Container->DoShake(ShakeIntensity, ContainerShakeDuration);
		}
	}

	KnockbackVelocity = Hit.ImpactNormal.GetSafeNormal() * KnockbackStrength * 100.f;
	const ShipCollisionType CollisionType = HitVelocity > OriginalMaxSpeed * MaxSpeedContainerFalloff ? ShipCollisionType::Heavy : ShipCollisionType::Light;	

	CollisionAudioComp->SetIntParameter(TEXT("CollisionType"), static_cast<int32>(CollisionType));
	CollisionAudioComp->Play();
	
	if (CollisionType == ShipCollisionType::Heavy)
		PopRandomContainerFromTop(Hit.ImpactNormal);
	
	CargoPlayerState->NotifyShipCollision(OtherActor, CollisionType);
	UE_LOG(LogTemp, Log, TEXT("Hit Velocity: %f / %f -> %.2f%% "), HitVelocity, OriginalMaxSpeed, HitVelocity / OriginalMaxSpeed)
}

void ACargoCharacter::PopRandomContainerFromTop(const FVector& HitDir)
{
	const auto HighestOccupiedZ = GridComp->GetHighestOccupiedZ();
	const auto PositionsTop = GridComp->GetPositionsFromLevel(HighestOccupiedZ);
	
	if (PositionsTop.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("No positions to pop"));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Popping random container"));
	const FVector ShipVelocity = FloatingMovement->Velocity;
	
	const auto RandomIndex = FMath::RandRange(0, PositionsTop.Num() - 1);
	const auto RandomPosition = PositionsTop[RandomIndex];	
	
	auto Placeable = GridComp->GetPlaceableAt(RandomPosition);		
	GridComp->RemovePlaceableFromGrid(Placeable);
	
	Placeable->FallIntoSea(HitDir, ShipVelocity);
}

void ACargoCharacter::PopContainersFromZ(int32 Z)
{	
	const auto PositionsTop = GridComp->GetPositionsFromLevel(Z);
	
	if (PositionsTop.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("No positions to pop on Z level: %d"), Z);
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Popping containers from Z: %d"), Z);
	
	for (const auto Position : PositionsTop)
	{
		auto Placeable = GridComp->GetPlaceableAt(Position);	
		
		if (Placeable == nullptr)
			continue;
		
		GridComp->RemovePlaceableFromGrid(Placeable);
	
		const FVector RandomDirection = FMath::VRandCone(FVector::UpVector,FMath::DegreesToRadians(25.0f));
		Placeable->FallIntoSea(RandomDirection, FloatingMovement->Velocity);
	}		
}

void ACargoCharacter::RotateShip(float TargetAngle)
{
	TargetRoll = TargetAngle;
}

void ACargoCharacter::OnEditModeChanged(bool bEditMode)
{
	if (bEditMode)
		StopMovementInput();

	if (bEditMode)
		GridComp->ShowIndicators();
	else
		GridComp->HideIndicators();	
}

void ACargoCharacter::InitializeShipName()
{
	ShipNameRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 2048, 512);

	ShipNameRenderTarget->ClearColor = FLinearColor::Transparent;

	ShipNameRenderTarget->OnCanvasRenderTargetUpdate.AddDynamic(this, &ThisClass::DrawShipName);

	ShipNameMaterial = ShipNameDecalComp->CreateDynamicMaterialInstance();

	ShipNameMaterial->SetTextureParameterValue(TEXT("ShipNameTexture"), ShipNameRenderTarget);

	ShipNameRenderTarget->UpdateResource();
}

void ACargoCharacter::OnShipNameChanged(FString NewShipName)
{
	ShipNameRenderTarget->UpdateResource();
}

void ACargoCharacter::DrawShipName(UCanvas* Canvas, int Width, int Height)
{
	const auto ShipName = CargoPlayerState->GetShipName();
	
	Canvas->K2_DrawText
	(
		ShipNameFont,
		ShipName,
		FVector2D(Width * 0.5f, Height * 0.5f),
		FVector2D(1.0f, 1.0f),
		FLinearColor::White,
		1.f,
		FLinearColor::Black,
		FVector2D::ZeroVector,
		true,
		true,
		false,
		FLinearColor::Black
	);
}

