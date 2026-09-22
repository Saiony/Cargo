// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CargoDropTarget.h"
#include "Components/ActorComponent.h"
#include "CoalChimney.generated.h"

class UAudioComponent;
class UNiagaraComponent;
class UStaticMeshComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API ACoalChimney : public AActor, public ICargoDropTarget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UNiagaraComponent> SmokeNiagaraComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UAudioComponent> AudioComp;

public:
	// Sets default values for this component's properties
	ACoalChimney();

	virtual void BeginDropHover(APlaceable* Placeable) override;
	virtual void EndDropHover() override;
	virtual void UpdateDropHover(APlaceable* Placeable, const FVector& ImpactPoint, APlaceablePreview* Preview) override;
	virtual bool TryAcceptDrop(APlaceable* Placeable, APlaceablePreview* Preview) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void PlaySmoke();
	
	void StopSmoke();
	
	
};
