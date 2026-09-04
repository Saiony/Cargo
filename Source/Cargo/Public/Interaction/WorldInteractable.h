#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldInteractable.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class USceneComponent;
class USoundBase;
struct FKey;

/**
 * Generic base class for world objects driven by mouse hover and click events.
 *
 * The PlayerController must have bEnableMouseOverEvents and bEnableClickEvents
 * enabled. The interaction component must also have suitable collision settings.
 */
UCLASS(Abstract, Blueprintable)
class CARGO_API AWorldInteractable : public AActor
{
	GENERATED_BODY()

public:
	AWorldInteractable();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Optional animation hook implemented by specialized interactables. */
	virtual void PlayInteractionAnimation();

	/** Object-specific action hook implemented by specialized interactables. */
	virtual void Interact();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	/** Parent for every component that should receive interaction outline feedback. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> InteractionVisualRoot;

	/** Dedicated collision volume that receives mouse hover and click events. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionHitbox;

	/** Primitive descendants of InteractionVisualRoot, cached at BeginPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Interaction")
	TArray<TObjectPtr<UPrimitiveComponent>> InteractionVisuals;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Audio")
	TObjectPtr<USoundBase> HoverSound;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Audio")
	TObjectPtr<USoundBase> ClickSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Outline", meta = (ClampMin = "0", ClampMax = "255"))
	int32 CustomDepthStencilValue = 1;

private:
	UFUNCTION()
	void HandleBeginCursorOver(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void HandleEndCursorOver(UPrimitiveComponent* TouchedComponent);	
	
	UFUNCTION()
	void HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
};
