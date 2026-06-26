// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CargoInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCargoInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface para objetos que podem ser interagidos no Cargo.
 */
class CARGO_API ICargoInteractable
{
	GENERATED_BODY()

public:
	/** Gatilha uma interação pelo Ator fornecido (geralmente o Jogador) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void Interact(AActor* Interactor);
};
