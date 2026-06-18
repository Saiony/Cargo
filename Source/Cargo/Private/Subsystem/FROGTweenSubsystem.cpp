// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/FROGTweenSubsystem.h"

void UFROGTweenSubsystem::Tick(const float DeltaSeconds)
{
	RotationTimeline.TickTimeline(DeltaSeconds);
}

UFROGTweenSubsystem::UFROGTweenSubsystem()
{
	ProgressUpdate.BindUFunction(this, FName("TimelineTick"));
	FinishedEvent.BindUFunction(this, FName("TimelineFinishedCallback"));
}

void UFROGTweenSubsystem::DoRotate(AActor* Actor, UCurveFloat* CurveFloat, const float TargetAngle, float Duration)
{
	Target = Actor;			
	
	if (!ensureAlways(CurveFloat))
		return;		
	
	CurveFloat->FloatCurve.Reset();
	CurveFloat->FloatCurve.UpdateOrAddKey(0.0f, Actor->GetActorRotation().Yaw); // Start angle
	CurveFloat->FloatCurve.AddKey(Duration, TargetAngle); // End angle
	
	RotationTimeline.AddInterpFloat(CurveFloat, ProgressUpdate);
	RotationTimeline.SetTimelineFinishedFunc(FinishedEvent);

	RotationTimeline.SetTimelineLength(Duration);
	RotationTimeline.PlayFromStart();
}

TStatId UFROGTweenSubsystem::GetStatId() const
{
	return TStatId();
}

void UFROGTweenSubsystem::TimelineTick(float Value)
{
}

void UFROGTweenSubsystem::TimelineFinishedCallback()
{
}

bool UFROGTweenSubsystem::IsTickableInEditor() const
{
	return false;
}

bool UFROGTweenSubsystem::IsTickableWhenPaused() const
{
	return Super::IsTickableWhenPaused();
}
