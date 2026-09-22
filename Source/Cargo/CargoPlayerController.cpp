// Copyright Epic Games, Inc. All Rights Reserved.


#include "CargoPlayerController.h"
#include "CargoCharacter.h"
#include "Public/Grid/Placeable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "CommonLocalPlayer.h"
#include "ConsoleVariables.h"
#include "Engine/OverlapResult.h"
#include "Grid/Container.h"
#include "Interaction/CargoInteractable.h"
#include "Runtime/Experimental/Voronoi/Private/voro++/src/container.hh"
#include "Subsystem/AudioSubsystem.h"
#include "Subsystem/CargoUIManagerSubsystem.h"
#include "Widgets/Input/SVirtualJoystick.h"

class UCargoUIManagerSubsystem;

ACargoPlayerController::ACargoPlayerController()
{
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
}

void ACargoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
	}
	
	GetWorldTimerManager().SetTimer(InteractionTimerHandle,this,&ACargoPlayerController::UpdateInteractionFocus,
								InteractionCheckInterval,true);

	const auto PlaceablePreviewClass = GetDefault<UCargoSettings>()->PlaceablePreviewClass.LoadSynchronous();
	PlaceablePreview = GetWorld()->SpawnActor<APlaceablePreview>(PlaceablePreviewClass, FVector::ZeroVector, FRotator::ZeroRotator);
	PlaceablePreview->SetActorHiddenInGame(true);
}

void ACargoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ACargoPlayerController::OnLeftClickStart);
			EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &ACargoPlayerController::OnLeftClickEnd);
			EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Started, this, &ACargoPlayerController::OnRightClick);
			EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Started, this, &ACargoPlayerController::OnCancel);
			EnhancedInputComponent->BindAction(SwitchCameraAction, ETriggerEvent::Completed, this, &ACargoPlayerController::SwitchEditMode);
			EnhancedInputComponent->BindAction(ToggleMapAction, ETriggerEvent::Completed, this, &ACargoPlayerController::OnToggleMap);			
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ACargoPlayerController::Interact);
			EnhancedInputComponent->BindAction(ScrollUp, ETriggerEvent::Completed, this, &ThisClass::OnScrollUp);
			EnhancedInputComponent->BindAction(ScrollDown, ETriggerEvent::Completed, this, &ThisClass::OnScrollDown);
		}
	}
}

bool ACargoPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void ACargoPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
	
	UpdateContainerHoverDetection(DeltaTime);

    if (!bEditMode)
        return;

    if (!bIsDragging || !DraggingObject)
        return;

	//gets mouse pos in world
    FVector MouseWorldLocation;
	FVector MouseWorldDirection;
	
    if (!DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
    {
        SetCurrentHovered(nullptr);
        return;
    }

	//placeable positioning
    const float T = (DraggingZHeight - MouseWorldLocation.Z) / MouseWorldDirection.Z;
    const FVector TargetLocation = MouseWorldLocation + MouseWorldDirection * T;
    DraggingObject->SetActorLocation(TargetLocation + DraggingOffset); //TODO: also use MouseWorldDirection.Rotation()

    FHitResult HitResult;
    GetHitResultUnderCursor(DropSurfaceChannel, false, HitResult);
    SetCurrentHovered(ResolveDropTarget(HitResult));

    if (!CurrentHovered)
        return;

    CurrentHovered->UpdateDropHover(DraggingObject, HitResult.ImpactPoint, PlaceablePreview);
    if (!PlaceablePreview->IsHidden())
        DraggingObject->SetActorRotation(PlaceablePreview->GetActorRotation() + FRotator(DraggingRotationOffset.X, DraggingRotationOffset.Y, DraggingRotationOffset.Z));
}

TScriptInterface<ICargoDropTarget> ACargoPlayerController::ResolveDropTarget(const FHitResult& HitResult) const
{
    // Components take precedence over their actors (for example, the ship's grid).
    if (Cast<ICargoDropTarget>(HitResult.GetComponent()))
        return TScriptInterface<ICargoDropTarget>(HitResult.GetComponent());

    AActor* HitActor = HitResult.GetActor();
    if (HitActor == DraggingObject)
        return nullptr;

    if (Cast<ICargoDropTarget>(HitActor))
        return TScriptInterface<ICargoDropTarget>(HitActor);

    // The visible cells belong to a child actor; the placeable handles stacking.
    if (APlaceableVisual* Visual = Cast<APlaceableVisual>(HitActor))
    {
        AActor* Owner = Visual->GetOwner();
        if (Owner != DraggingObject && Cast<ICargoDropTarget>(Owner))
            return TScriptInterface<ICargoDropTarget>(Owner);
    }

    return nullptr;
}

void ACargoPlayerController::SetCurrentHovered(TScriptInterface<ICargoDropTarget> NewHovered)
{
    if (CurrentHovered.GetObject() == NewHovered.GetObject())
        return;

    if (IsValid(CurrentHovered.GetObject()))
        CurrentHovered->EndDropHover();

    CurrentHovered = NewHovered;
    if (PlaceablePreview)
        PlaceablePreview->SetActorHiddenInGame(true);

    if (IsValid(CurrentHovered.GetObject()))
        CurrentHovered->BeginDropHover(DraggingObject);
}

void ACargoPlayerController::UpdateContainerHoverDetection(float DeltaTime)
{
	FHitResult HitResult;
	
	if (!GetHitResultUnderCursor(PlaceableChannel, false, HitResult))
	{
		OnContainerHoverConfirmed.Broadcast(nullptr);
		return;
	}
	
	const auto Container = Cast<AContainer>(HitResult.GetActor()->GetOwner());
	
	if (!Container)
	{		
		OnContainerHoverConfirmed.Broadcast(nullptr);
		return;
	}
	
	if (CurrentHoveredContainer != Container)
	{
		CurrentHoveredContainer = Container;
		ContainerHoverElapsedTime = 0.f;			
		OnContainerHoverConfirmed.Broadcast(nullptr);
		return;
	}
	
	CurrentHoveredContainer = Container;
	ContainerHoverElapsedTime += DeltaTime;
	
	if (ContainerHoverElapsedTime < ContainerHoverThreshold)
		return;
	
	ContainerHoverElapsedTime = 0.f;	
	OnContainerHoverConfirmed.Broadcast(Cast<AContainer>(Container));
}

void ACargoPlayerController::OnLeftClickStart(const FInputActionValue& Value)
{
	FHitResult HitResult;
	
	if (!bEditMode)
		return;
	
	if (!GetHitResultUnderCursor(PlaceableChannel, false, HitResult))
		return;
	
	APlaceable* Placeable = Cast<APlaceable>(HitResult.GetActor());
	
	if (!Placeable)
	{
		Placeable = Cast<APlaceable>(HitResult.GetActor()->GetParentActor());
		
		if (!Placeable)
			return;
	}
	
	DraggingObject = Placeable;
	if (Placeable->IsPlaceableBlocked(Placeable))
	{
		UE_LOG(LogTemp, Warning, TEXT("Placeable is blocked"));
		return;
	}
	
	SetCurrentHovered(nullptr);
	bIsDragging = true;
		
	DraggingObject->Grab();
	DraggingObject->AlignToRotation(GetPawn()->GetActorRotation());
		
	PlaceablePreview->Initialize(DraggingObject);			
}

void ACargoPlayerController::OnLeftClickEnd(const FInputActionValue& InputActionValue)
{
	if (!bEditMode)
		return;

	if (!bIsDragging || !DraggingObject)
		return;	
	
    if (IsValid(CurrentHovered.GetObject()))
    {
        if (!CurrentHovered->TryAcceptDrop(DraggingObject, PlaceablePreview))
            return;
    }
    else
    {
        DraggingObject->Release();
    }

    SetCurrentHovered(nullptr);
    PlaceablePreview->SetActorHiddenInGame(true);
    bIsDragging = false;
    DraggingObject = nullptr;
}

void ACargoPlayerController::OnRightClick(const FInputActionValue& Value)
{
	if (!bEditMode)
		return;
	
	if (!bIsDragging)
		return;
	
	if (DraggingObject)
		DraggingObject->RotateClockwise();
	
	if (PlaceablePreview)
		PlaceablePreview->MimicPlaceableYaw(DraggingObject);
}

void ACargoPlayerController::OnCancel(const FInputActionValue& Value)
{
}

void ACargoPlayerController::SwitchEditMode(const FInputActionValue& Value)
{
	SetCurrentHovered(nullptr);
	bEditMode = !bEditMode;
	bShowMouseCursor = bEditMode;

	if (bEditMode)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
	}
	
	OnEditModeChanged.Broadcast(bEditMode);
}

void ACargoPlayerController::OnToggleMap(const FInputActionInstance& InputActionInstance)
{
	GetGameInstance()->GetSubsystem<UCargoUIManagerSubsystem>()->ToggleMap();
}

void ACargoPlayerController::Interact(const FInputActionValue& InputActionValue)
{
	if (bEditMode)
		return;
	
	const auto ControlledPawn = GetPawn();

	const FVector Origin = ControlledPawn->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ControlledPawn);

	const bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(OverlapResults, Origin, FQuat::Identity,
													ECC_WorldDynamic, FCollisionShape::MakeSphere(InteractRange),
																QueryParams );
	
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (CVarCargoShowDebugs.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), Origin, InteractRange, 16, FColor::Green,
						false, 1.5f, 0, 1.0f);
	}
#endif

	if (!bHasOverlaps)
		return;

	AActor* BestTarget = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	//finds the one closer to the player
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* Actor = Overlap.GetActor();
		if (!Actor || !Actor->GetClass()->ImplementsInterface(UCargoInteractable::StaticClass()))
			continue;

		const float DistSq = FVector::DistSquared(Origin, Actor->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Actor;
		}
	}

	if (BestTarget)
		ICargoInteractable::Execute_Interact(BestTarget, ControlledPawn);
	else
		UE_LOG(LogTemp, Log, TEXT("Nothing to interact with"));
}

void ACargoPlayerController::OnScrollUp(const FInputActionValue& InputActionValue)
{
}

void ACargoPlayerController::OnScrollDown(const FInputActionValue& InputActionValue)
{
}

void ACargoPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	GetGameInstance()->GetSubsystem<UCargoUIManagerSubsystem>()->NotifyPlayerAdded(Cast<UCommonLocalPlayer>(GetWorld()->GetFirstLocalPlayerFromController()));
}

void ACargoPlayerController::UpdateInteractionFocus()
{
	TScriptInterface<ICargoInteractable> NewTarget = FindBestInteractable();

	if (NewTarget.GetObject() != CurrentInteractable.GetObject())
	{
		if (CurrentInteractable != nullptr)
		{
			UAudioSubsystem::Get(this)->SetBGMVolume(1.0f);	
			CurrentInteractable->Unfocus();
		}
		
		CurrentInteractable = NewTarget.GetObject();
		
		if (CurrentInteractable != nullptr)
		{
			UAudioSubsystem::Get(this)->SetBGMVolume(0.3f);
			CurrentInteractable->Focus();
		}
		
		OnInteractableChanged.Broadcast(NewTarget);
	}
}

TScriptInterface<ICargoInteractable> ACargoPlayerController::FindBestInteractable() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
		return nullptr;

	const FVector Origin = ControlledPawn->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ControlledPawn);

	const bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(
		OverlapResults, Origin, FQuat::Identity,
		ECC_WorldDynamic, FCollisionShape::MakeSphere(InteractRange),
		QueryParams
	);

	if (!bHasOverlaps)
		return nullptr;

	AActor* BestTarget = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* Actor = Overlap.GetActor();
		if (!Actor || !Actor->GetClass()->ImplementsInterface(UCargoInteractable::StaticClass()))
			continue;

		const float DistSq = FVector::DistSquared(Origin, Actor->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Actor;
		}
	}

	return BestTarget;
}

void ACargoPlayerController::StartDragging(APlaceable* InPlaceable)
{
	if (!InPlaceable)
	{
		return;
	}

	SetCurrentHovered(nullptr);

	if (DraggingObject && DraggingObject != InPlaceable)
	{
		DraggingObject->Destroy();
	}

	DraggingObject = InPlaceable;
	bIsDragging = true;

	DraggingObject->Grab();
	if (APawn* ControlledPawn = GetPawn())
	{
		DraggingObject->AlignToRotation(ControlledPawn->GetActorRotation());
	}

	if (!PlaceablePreview)
	{
		const auto PreviewClass = GetDefault<UCargoSettings>()->PlaceablePreviewClass.LoadSynchronous();
		if (PreviewClass)
		{
			PlaceablePreview = GetWorld()->SpawnActor<APlaceablePreview>(PreviewClass, FVector::ZeroVector, FRotator::ZeroRotator);
		}
	}

	if (PlaceablePreview)
	{
		PlaceablePreview->Initialize(DraggingObject);
	}

	if (!bEditMode)
	{
		bEditMode = true;
		bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}

	FVector MouseWorldLocation;
	FVector MouseWorldDirection;
	if (DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
	{
		const float T = (DraggingZHeight - MouseWorldLocation.Z) / (FMath::IsNearlyZero(MouseWorldDirection.Z) ? 1.0f : MouseWorldDirection.Z);
		const FVector TargetLocation = MouseWorldLocation + MouseWorldDirection * T;
		DraggingObject->SetActorLocation(TargetLocation);
	}
}
