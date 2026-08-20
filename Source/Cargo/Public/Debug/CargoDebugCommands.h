// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "CargoDebugCommands.generated.h"

class UContainerDA;

UCLASS()
class CARGO_API UCargoDebugCommands : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cargo|Debug")
	static void LoadShip(UObject* WorldContextObject, const FString& CargoType);

	UFUNCTION(BlueprintCallable, Category = "Cargo|Debug")
	static void GetContainer(UObject* WorldContextObject, const FString& CargoType);

	static void LoadShipConsole(const TArray<FString>& Args, UWorld* World);
	static void GetContainerConsole(const TArray<FString>& Args, UWorld* World);

private:
	static UContainerDA* ResolveContainerDataAsset(const FString& CargoTypeStr, FGameplayTag* OutTag = nullptr);
};
