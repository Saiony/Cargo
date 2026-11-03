// Copyright Epic Games, Inc. All Rights Reserved.

#include "TurtleEnemy.h"

#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ATurtleEnemy::ATurtleEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	MovementSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MovementSpline"));
	MovementSpline->SetupAttachment(RootComponent);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bOrientRotationToMovement = false;
		CharacterMovement->bUseControllerDesiredRotation = false;
	}
}

void ATurtleEnemy::SetSplineToFollow(USplineComponent* InSpline)
{
	SplineToFollow = InSpline;
}

void ATurtleEnemy::BeginPlay()
{
	Super::BeginPlay();

	USplineComponent* PathSpline = IsValid(SplineToFollow) ? SplineToFollow.Get() : MovementSpline.Get();
	if (!IsValid(PathSpline))
	{
		return;
	}

	// The turtle's own spline is authored as a child for convenient level editing,
	// but must stay fixed in the world while the character follows it.
	if (PathSpline == MovementSpline && PathSpline->GetAttachParent() != nullptr)
	{
		PathSpline->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	SplineToFollow = PathSpline;
	SplineWorldTransform = PathSpline->GetComponentTransform();
	SplineLength = PathSpline->GetSplineLength();
	bIsSplineClosed = PathSpline->IsClosedLoop();
	bHasValidPath = SplineLength > KINDA_SMALL_NUMBER;
	DistanceAlongSpline = 0.0f;
	// Movement is driven by swept spline-to-spline steps below, not by acceleration or gravity.
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	if (bHasValidPath)
	{
		const FVector StartLocation = SplineWorldTransform.TransformPosition(
			PathSpline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::Local));
		const FVector Offset = StartLocation - GetActorLocation();
		if (!Offset.IsNearlyZero())
		{
			// Use the character movement component so the initial alignment still sweeps for collision.
			GetCharacterMovement()->MoveUpdatedComponent(Offset, GetActorQuat(), true);
		}
	}
}

void ATurtleEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHasValidPath || DeltaTime <= 0.0f || SplineSpeed <= 0.0f)
	{
		return;
	}
	USplineComponent* PathSpline = SplineToFollow.Get();
	if (!IsValid(PathSpline))
	{
		Destroy();
		return;
	}

	const float PreviousDistance = DistanceAlongSpline;
	const float NextDistance = PreviousDistance + SplineSpeed * DeltaTime;
	if (bIsSplineClosed)
	{
		DistanceAlongSpline = FMath::Fmod(NextDistance, SplineLength);
	}
	else
	{
		DistanceAlongSpline = FMath::Min(NextDistance, SplineLength);
	}

	const FVector LocalLocation = PathSpline->GetLocationAtDistanceAlongSpline(
		DistanceAlongSpline, ESplineCoordinateSpace::Local);
	const FVector WorldLocation = SplineWorldTransform.TransformPosition(LocalLocation);
	const FVector LocalDirection = PathSpline->GetDirectionAtDistanceAlongSpline(
		DistanceAlongSpline, ESplineCoordinateSpace::Local);
	const FVector WorldDirection = SplineWorldTransform.TransformVectorNoScale(LocalDirection).GetSafeNormal();
	if (WorldDirection.IsNearlyZero())
	{
		return;
	}

	const FRotator DesiredRotation = WorldDirection.Rotation();
	const FRotator SmoothedRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaTime, RotationInterpSpeed);
	FHitResult MoveHit;
	GetCharacterMovement()->MoveUpdatedComponent(WorldLocation - GetActorLocation(), SmoothedRotation, true, &MoveHit);

	if (MoveHit.IsValidBlockingHit())
	{
		// Keep spline progress in step with the swept movement if an obstacle blocks the character.
		const float BlockedDistance = PreviousDistance + SplineSpeed * DeltaTime * MoveHit.Time;
		DistanceAlongSpline = bIsSplineClosed
			? FMath::Fmod(BlockedDistance, SplineLength)
			: FMath::Min(BlockedDistance, SplineLength);
		return;
	}

	if (!bIsSplineClosed && DistanceAlongSpline >= SplineLength)
	{
		Destroy();
	}
}
