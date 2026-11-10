// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoneArch.generated.h"

class UBoxComponent;

UCLASS()
class CARGO_API AStoneArch : public AActor
{
	GENERATED_BODY()
	
protected:	
	UPROPERTY(EditAnywhere, Category="Cargo")
	int8 Height;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UBoxComponent> DetectionBox;
	
	virtual void BeginPlay() override;
	
public:	
	AStoneArch();
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
