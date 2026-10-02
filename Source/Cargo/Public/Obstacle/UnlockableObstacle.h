// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CargoInteractable.h"
#include "UI/Shop/SimplePurchaseWidget.h"
#include "UnlockableObstacle.generated.h"

class UWidgetComponent;

UCLASS()
class CARGO_API AUnlockableObstacle : public AActor, public ICargoInteractable, public ISimplePurchaseWidgetListener
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UWidgetComponent> InteractableWidgetComp;
	
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category="Cargo")
	TObjectPtr<UStaticMeshComponent> ObstacleMeshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cargo")
	TSubclassOf<USimplePurchaseWidget> SimplePurchaseWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cargo")
	int32 CostToUnlock = 50;

	virtual void BeginPlay() override;
	
	void Unlock();
	
public:
	AUnlockableObstacle();
		
	virtual void Tick(float DeltaTime) override;
	
	virtual void Focus() override;
	virtual void Unfocus() override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void OnSimplePurchaseWidgetCallback(bool Purchased) override;
};
