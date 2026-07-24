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

class CARGO_API ICargoInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void Interact(AActor* Interactor);
	
	UFUNCTION()
	virtual void Focus() = 0;
	
	UFUNCTION()
	virtual void Unfocus() = 0;
};