// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable.h"
#include "DataAssets/ContainerDA.h"
#include "Container.generated.h"

UCLASS()
class CARGO_API AContainer : public APlaceable
{
	GENERATED_BODY()

	void Init();
public:
	// Sets default values for this actor's properties
	AContainer();
	
	UPROPERTY(EditAnywhere, Category="Cargo")
	TObjectPtr<UContainerDA> ContainerDA;

	void Init(TObjectPtr<UContainerDA> InContainerDA);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
};
