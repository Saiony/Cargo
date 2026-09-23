// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interaction/CargoDropTarget.h"
#include "Components/ActorComponent.h"
#include "CoalChimney.generated.h"

class UAudioComponent;
class UNiagaraComponent;
class UStaticMeshComponent;
class ACargoCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API ACoalChimney : public AActor, public ICargoDropTarget
{
	GENERATED_BODY()
protected:	
	TWeakObjectPtr<ACargoCharacter> Ship;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<USceneComponent> RootSceneComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UNiagaraComponent> SmokeNiagaraComp;

	UPROPERTY(EditDefaultsOnly, Category="Cargo|Smoke", meta=(ClampMin="0.0", Units="s"))
	float SmokeFadeDuration = 0.5f;

	float SmokeIntensity = 0.f;
	float TargetSmokeIntensity = 0.f;
	void SetSmokeTarget(float Intensity);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UAudioComponent> AudioComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	FGameplayTag CoalTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo", meta=(ClampMin="0.0"))
	float FuelPerContainer = 25.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UMaterialInterface> HoverMaterial_Right;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UMaterialInterface> HoverMaterial_Wrong;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	int32 FuelPerCoal = 50;
	
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> OriginalMaterial;


public:
	// Sets default values for this component's properties
	ACoalChimney();

	virtual void BeginDropHover(APlaceable* Placeable) override;
	virtual void EndDropHover() override;
	virtual void UpdateDropHover(APlaceable* Placeable, const FVector& ImpactPoint, APlaceablePreview* Preview) override;
	virtual bool TryAcceptDrop(APlaceable* Placeable, APlaceablePreview* Preview) override;
	
protected:	
	UFUNCTION(BlueprintImplementableEvent, Category="Cargo|Animation")
	void OnStartHover();

	UFUNCTION(BlueprintImplementableEvent, Category="Cargo|Animation")
	void OnEndHover();

	UFUNCTION(BlueprintImplementableEvent, Category="Cargo|Animation")
	void OnCoalAdded();

	void PlaySmoke();
	
	void StopSmoke();	
};
