// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CargoInteractable.h"
#include "CargoGameMode.h"
#include "GameplayTagContainer.h"
#include "Grid/Container.h"
#include "Port/CargoPort.h"
#include "CargoIsland.generated.h"

class UWidgetComponent;

UCLASS()
class CARGO_API ACargoIsland : public AActor, public ICargoInteractable
{
	GENERATED_BODY()
	
public:	
	ACargoIsland();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island", meta = (Categories = "Location"))
	FGameplayTag LocationTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island", meta = (Categories = "Dialogue"))
	FGameplayTag InteractionDialogueTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	TObjectPtr<UStaticMeshComponent> IslandMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	TObjectPtr<UCargoPortComponent> PortComponent;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	TObjectPtr<UWidgetComponent> InteractableWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo|Island")
	TArray<TObjectPtr<AContainer>> SpawnedContainers;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo|Island")
	TSubclassOf<AContainer> ContainerClass;	

public:
	// ICargoInteractable Interface
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	virtual void Focus() override;
	
	virtual void Unfocus() override;

	void OnQuestAccepted(TObjectPtr<UQuestData> QuestData, AActor* QuestInstigator);	
	
	void OnQuestCompleted(TObjectPtr<UQuestStatus> QuestStatus);

	UFUNCTION(BlueprintPure, Category="Cargo|Island")
	FGameplayTag GetLocationTag() const { return LocationTag; }
};
