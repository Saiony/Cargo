// Copyright Epic Games, Inc. All Rights Reserved.

#include "CargoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Cargo.h"
#include "BuoyancyComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Grid/Placeable.h"


ACargoCharacter::ACargoCharacter()
{
	// Set size for collision capsule
	RootMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMeshComp"));
	SetRootComponent(RootMeshComponent);	
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	
	DeckMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeckComp"));
	DeckMeshComponent->SetupAttachment(MeshComponent);

	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
	FloatingMovement->MaxSpeed = 600.f;
	FloatingMovement->Acceleration = 400.f;
	FloatingMovement->Deceleration = 800.f;

	BuoyancyComp = CreateDefaultSubobject<UBuoyancyComponent>("BuoyancyComp");
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;	
	

	// Configure character movement
	// GetCharacterMovement()->bOrientRotationToMovement = true;
	// GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	// GetCharacterMovement()->JumpZVelocity = 500.f;
	// GetCharacterMovement()->AirControl = 0.35f;
	// GetCharacterMovement()->MaxWalkSpeed = 500.f;
	// GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	// GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	// GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	// GameplayCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("GameplayCameraBoom"));
	// GameplayCameraBoom->SetupAttachment(RootComponent);
	// GameplayCameraBoom->TargetArmLength = 400.0f;
	// GameplayCameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	// GameplayCamera = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("GameplayCamera"));
	// GameplayCamera->SetupAttachment(RootComponent);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
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
	//AddMovementInput(RightDirection, Right);

	// Rotation
	if (Right != 0.f)
	{
		const float DeltaTime = GetWorld()->GetDeltaSeconds();
	
		FRotator Delta(0.f, Right * RotationSpeed * DeltaTime, 0.f);
		AddActorLocalRotation(Delta);
	}
	if (GetController() == nullptr)
		return;
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
	IGridActorInterface::OnPlaceableAdded(Placeable);
	
	BalanceShip();
}

void ACargoCharacter::OnPlaceableGrabbed_Implementation(APlaceable* Placeable)
{	
	Placeable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	RemovePlaceableFromGrid(Placeable);	
	
	BalanceShip();
}

void ACargoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	DrawDebugGrid(0.0f);
}

void ACargoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeGrid(GetDefault<UCargoSettings>()->GridCellSize, FIntPoint(0, 0), FIntPoint(10, 10));
}

void ACargoCharacter::BalanceShip()
{
	FR = 0;
	for (auto PlaceableKV : PlaceableGrid.GetOccupiedSlots())
	{
		const auto PlaceableWeight = PlaceableKV.Value->Weight;
		const auto Momentum = PlaceableWeight * PlaceableKV.Key.Y;
		
		FR += Momentum;
	}
	
	UE_LOG(LogTemp, Log, TEXT("FR: %f"), FR);
	
	float FinalAngle = FMath::GetMappedRangeValueClamped(
	FVector2D(-1000.f, 1000.f),
	FVector2D(-30.f, 30.f),
	FR);
	
	RotateShip(FinalAngle);
}
