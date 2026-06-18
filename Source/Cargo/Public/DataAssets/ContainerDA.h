// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ContainerDA.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UContainerDA : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	TObjectPtr<UMaterialInterface> Material;
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	float Weight = 1.0f;	
};
