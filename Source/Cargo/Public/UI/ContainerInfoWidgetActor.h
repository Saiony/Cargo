// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/Container.h"
#include "ContainerInfoWidgetActor.generated.h"

class UContainerInfoWidget;
class UWidgetComponent;

UCLASS()
class CARGO_API AContainerInfoWidgetActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Cargo")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Cargo")
	TSubclassOf<UContainerInfoWidget> WidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "Cargo")
	FVector Offset;

public:
	// Sets default values for this actor's properties
	AContainerInfoWidgetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleContainerHoverConfirmed(AContainer* HoveredContainer);
	void Hide();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
