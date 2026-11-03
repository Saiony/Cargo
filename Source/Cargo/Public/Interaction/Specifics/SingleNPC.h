// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dialogue/DialogueData.h"
#include "GameFramework/Actor.h"
#include "Interaction/CargoInteractable.h"
#include "SingleNPC.generated.h"

class UWidgetComponent;

UCLASS()
class CARGO_API ASingleNPC : public AActor, public ICargoInteractable
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UWidgetComponent> InteractableWidgetComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UDialogueData> DialogueData;
	
public:
	ASingleNPC();

	virtual void Focus() override;
	virtual void Unfocus() override;
	virtual void Interact_Implementation(AActor* Interactor) override;
};
