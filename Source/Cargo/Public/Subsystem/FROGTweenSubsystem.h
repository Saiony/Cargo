// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "FROGTweenSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UFROGTweenSubsystem : public UTickableWorldSubsystem
{
private:
	GENERATED_BODY()
	AActor* Target = nullptr;	
	
	FTimeline RotationTimeline;

public:
	FOnTimelineFloat ProgressUpdate;
	FOnTimelineEvent FinishedEvent;

	UFROGTweenSubsystem();

	UFUNCTION(BlueprintCallable)
	void DoRotate(AActor* Actor, UCurveFloat* CurveFloat, const float TargetAngle, float Duration);
	
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;

	UFUNCTION()
	void TimelineTick(float Value);

	UFUNCTION()
	void TimelineFinishedCallback();	
};
