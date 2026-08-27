// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "Grid/GridComponent.h"
#include "Grid/PlaceablePreview.h"
#include "Interaction/CargoInteractable.h"
#include "CargoPlayerController.generated.h"

class AContainer;
class UInputMappingContext;
class UInputAction;
class UUserWidget;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableChanged, TScriptInterface<ICargoInteractable>, NewInteractable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContainerHoverConfirmed, AContainer*, HoveredContainer);

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class ACargoPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LeftClickAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* RightClickAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* CancelAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SwitchCameraAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ToggleMapAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* InteractAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ScrollUp;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ScrollDown;	

	UPROPERTY(EditAnywhere, Category="Input|Dragging")
	float DraggingZHeight = 100.0f;
	
	UPROPERTY(EditAnywhere, Category="Input|Dragging")
	FVector3d DraggingOffset;
	
	UPROPERTY(EditAnywhere, Category="Input|Dragging")
	FVector3d DraggingRotationOffset;

	UPROPERTY(VisibleAnywhere, Category="Input|Dragging")
	class APlaceable* DraggingObject;

	UPROPERTY(VisibleAnywhere, Category="Input|Dragging")
	bool bIsDragging = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float InteractRange = 1000;
	
	UPROPERTY(EditAnywhere, Category="Cargo")
	TEnumAsByte<ECollisionChannel> DropSurfaceChannel;
	
	UPROPERTY(EditAnywhere, Category="Cargo")
	TEnumAsByte<ECollisionChannel> PlaceableChannel;
	
	UPROPERTY(EditDefaultsOnly, Category="Cargo")
	float InteractionCheckInterval = 0.1f;
	
	FTimerHandle InteractionTimerHandle;
	
	UPROPERTY()
	TScriptInterface<ICargoInteractable> CurrentInteractable = nullptr;
	
	UPROPERTY()
	TObjectPtr<UGridComponent> CurrentHoveredGrid = nullptr;
	
	UPROPERTY()
	TObjectPtr<APlaceablePreview> PlaceablePreview;
	
	/** Gameplay initialization */
	virtual void BeginPlay() override;

	virtual void PlayerTick(float DeltaTime) override;
	
	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

	void OnLeftClickStart(const FInputActionValue& Value);
	void OnLeftClickEnd(const FInputActionValue& InputActionValue);
	void OnRightClick(const FInputActionValue& Value);
	void OnCancel(const FInputActionValue& Value);
	void SwitchEditMode(const FInputActionValue& Value);
	void OnToggleMap(const FInputActionInstance& InputActionInstance);
	
	void Interact(const FInputActionValue& InputActionValue);
	void OnScrollUp(const FInputActionValue& InputActionValue);
	void OnScrollDown(const FInputActionValue& InputActionValue);
	
	void OnPossess(APawn* InPawn) override;	
	
	void UpdateInteractionFocus();

	TScriptInterface<ICargoInteractable> FindBestInteractable() const;
	
	void UpdateContainerHoverDetection(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category = "ContainerInfo")
	float ContainerHoverThreshold = 1.0f;
	
	float ContainerHoverElapsedTime = 0.0f;
	
	UPROPERTY()
	TWeakObjectPtr<AContainer> CurrentHoveredContainer;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input")
	bool bEditMode = false;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteractableChanged OnInteractableChanged;
	
	FOnContainerHoverConfirmed OnContainerHoverConfirmed;

	void StartDragging(class APlaceable* InPlaceable);
};
