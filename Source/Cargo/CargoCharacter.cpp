// Copyright Epic Games, Inc. All Rights Reserved.

#include "CargoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Cargo.h"
#include "BuoyancyComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
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
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	auto SideTilt = WeightInbalanceMultiplier * FR / 10000;

	// route the input
	DoMove(MovementVector.X + SideTilt, MovementVector.Y);
}

void ACargoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACargoCharacter::DoMove(float Right, float Forward)
{
	if (GetController() == nullptr)
		return;
	
	// Movement relative to actor
	const FVector ForwardDirection = GetActorForwardVector();
	const FVector RightDirection   = GetActorRightVector();

	AddMovementInput(ForwardDirection, Forward);

	// Rotation
	if (Right != 0.f)
	{
		const float DeltaTime = GetWorld()->GetDeltaSeconds();
	
		FRotator Delta(0.f, Right * RotationSpeed * DeltaTime, 0.f);
		AddActorLocalRotation(Delta);
	}
}

void ACargoCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACargoCharacter::AttachPlaceable(APlaceable* Placeable, FVector WorldPos)
{    
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	Placeable->AttachToActor(this, AttachmentRules);
}

void ACargoCharacter::OnPlaceableAdded(APlaceable* Placeable)
{
	BalanceShip();
}

void ACargoCharacter::OnPlaceableRemoved(APlaceable* Placeable)
{	
	BalanceShip();
}

void ACargoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateEngineSoundIntensity();
}

void ACargoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GridComp->OnPlaceableAddedToGrid.AddDynamic(this, &ThisClass::OnPlaceableAdded);
	GridComp->OnPlaceableRemovedFromGrid.AddDynamic(this, &ThisClass::OnPlaceableRemoved);
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
	RotateShip(FinalAngle);
}

void ACargoCharacter::UpdateEngineSoundIntensity()
{
	const float SpeedValue = FloatingMovement->Velocity.Size2D();
	const float NormalizedSpeed = FMath::Clamp(SpeedValue / FloatingMovement->GetMaxSpeed(), 0.f, 1.f);

	MovementAudioComp->SetFloatParameter(FName("Speed"), NormalizedSpeed);
}


void ACargoCharacter::OnHasteCVarChanged(IConsoleVariable* ConsoleVariable)
{
	const bool Haste = CVarBoostMovement.GetValueOnGameThread();

	FloatingMovement->MaxSpeed = Haste ? FloatingMovement->MaxSpeed * 2 : FloatingMovement->MaxSpeed / 2;
	FloatingMovement->Acceleration = Haste ? FloatingMovement->Acceleration * 2 : FloatingMovement->Acceleration / 2;
}