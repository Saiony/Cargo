// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurtleEnemy.generated.h"

class USplineComponent;

/** Character that follows an editable spline, looping only when the spline is closed. */
UCLASS()
class ATurtleEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ATurtleEnemy();

	/** Selects a spawner-owned spline before this actor begins play. */
	void SetSplineToFollow(USplineComponent* InSpline);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Editable world path followed by this character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USplineComponent> MovementSpline;

	/** Optional external path supplied by a spawner. */
	UPROPERTY(Transient)
	TObjectPtr<USplineComponent> SplineToFollow;

	/** Travel speed along the spline, in cm/s. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0", Units="cm/s"))
	float SplineSpeed = 150.0f;

	/** Interpolation speed used to turn toward the path direction. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
	float RotationInterpSpeed = 5.0f;

	/** Distance already traveled along the spline, in centimeters. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Movement", meta=(Units="cm"))
	float DistanceAlongSpline = 0.0f;

	/** Frozen transform keeps the authored path fixed while the character moves. */
	FTransform SplineWorldTransform = FTransform::Identity;
	float SplineLength = 0.0f;
	bool bIsSplineClosed = false;
	bool bHasValidPath = false;
};
