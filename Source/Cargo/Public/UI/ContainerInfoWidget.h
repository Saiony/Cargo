// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Components/TextBlock.h"
#include "ContainerInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARGO_API UContainerInfoWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CargoText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WeightText;
	
public:
	void Show(const FString& Cargo, float Weight, const FVector& WorldPos);
	
	void Hide();	
};
