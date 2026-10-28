// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MilitarBarrier.generated.h"

class UBoxComponent;
class UDialogueData;

UCLASS()
class CARGO_API AMilitarBarrier : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMilitarBarrier();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UBoxComponent> DetectionBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UBoxComponent> BlockingBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UDialogueData> DialogueData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	int8 RequiredPatent;

	UFUNCTION()
	void OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
};
