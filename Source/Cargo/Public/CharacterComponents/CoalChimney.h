// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CargoPlayerController.h"
#include "../../../../../../../Program Files/Epic Games/UE_5.7/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "Components/ActorComponent.h"
#include "CoalChimney.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API ACoalChimney : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UNiagaraComponent> SmokeNiagaraComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UAudioComponent> AudioComp;

public:
	// Sets default values for this component's properties
	ACoalChimney();

	void OnHover(TObjectPtr<APlaceable> Placeable);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void PlaySmoke();
	
	void StopSmoke();
	
	
	void OnUnhover();
};
