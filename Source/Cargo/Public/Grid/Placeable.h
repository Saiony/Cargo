// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CargoCharacter.h"
#include "GameFramework/Actor.h"
#include "Placeable.generated.h"

UCLASS()
class CARGO_API APlaceable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlaceable();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;
	
	TScriptInterface<IGridActorInterface> OwningGridActor;
	
	FIntPoint GridPos;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Grab();
	
	void Init(TScriptInterface<IGridActorInterface> GridActor, int32 GridPosX, int32 GridPosY);
};
