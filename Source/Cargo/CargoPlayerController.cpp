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
        return;

    const float T = (DraggingZHeight - MouseWorldLocation.Z) / MouseWorldDirection.Z;
    const FVector TargetLocation = MouseWorldLocation + MouseWorldDirection * T;
    DraggingObject->SetActorLocation(TargetLocation);

	//gets hit component
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(DraggingObject);

    const FVector TraceStart = DraggingObject->GetActorLocation();
    const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, 10000.f);

    const auto HitComponent = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, DropSurfaceChannel, Params)
						        ? HitResult.GetComponent()
						        : nullptr;

    if (!HitComponent)
    {     
        PlaceablePreview->SetActorHiddenInGame(true);
    	CurrentHoveredGrid = nullptr;
        return;
    }

    UGridComponent* GridComponent = Cast<UGridComponent>(HitComponent);
	FVector ImpactPoint = HitResult.ImpactPoint;	

	CurrentHoveredGrid = GridComponent;

    if (!GridComponent)
    {    	
    	if (auto Container = Cast<AContainer>(HitComponent->GetOwner()))
    	{
    		UE_LOG(LogTemp, Log, TEXT("HitComponent is a container. OriginalPos:"));    		
    		CurrentHoveredGrid = Container->OwningGridActor;
    		ImpactPoint = CurrentHoveredGrid->GetNextFreeZPositionWorld(ImpactPoint);
    	}
    	else
    	{
	        PlaceablePreview->SetActorHiddenInGame(true);
	        return;
    	}
    }  

    if (CurrentHoveredGrid->CanAddPlaceableToGrid(DraggingObject, ImpactPoint, DraggingObject->GetLocalYaw()))
    {
        PlaceablePreview->SetValid();
    }
    else
    {
        PlaceablePreview->SetInvalid();
    }
    
    PlaceablePreview->SetActorHiddenInGame(false);

    PlaceablePreview->AttachToComponent(CurrentHoveredGrid, FAttachmentTransformRules::SnapToTargetIncludingScale);	

    FVector LocalLocation = CurrentHoveredGrid->GetComponentTransform().InverseTransformPosition(ImpactPoint);

    const float GridSize = GetDefault<UCargoSettings>()->GridCellSize;

    LocalLocation.X = FMath::GridSnap(LocalLocation.X, GridSize);
    LocalLocation.Y = FMath::GridSnap(LocalLocation.Y, GridSize);
    LocalLocation.Z = FMath::GridSnap(LocalLocation.Z, GridSize);

    PlaceablePreview->SetActorRelativeLocation(LocalLocation);
	PlaceablePreview->SetActorRelativeRotation(FRotator(0.f, 0.f, 0.f));
	PlaceablePreview->MimicPlaceableYaw(DraggingObject);
}

void ACargoPlayerController::UpdateContainerHoverDetection(float DeltaTime)
{
	FHitResult HitResult;
	
	if (!GetHitResultUnderCursor(DropSurfaceChannel, false, HitResult))
		return;
	
	const auto Container = Cast<AContainer>(HitResult.GetActor());
	
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
	
	if (!GetHitResultUnderCursor(DropSurfaceChannel, false, HitResult))
		return;
	
	APlaceable* Placeable = Cast<APlaceable>(HitResult.GetActor());
	
	if (!Placeable)
		return;
	
	DraggingObject = Placeable;
	if (Placeable->IsPlaceableBlocked(Placeable))
	{
		UE_LOG(LogTemp, Warning, TEXT("Placeable is blocked"));
		return;
	}
	
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
	
	//if no grid below, just drop with physics
	if (!CurrentHoveredGrid)
	{		
		DraggingObject->Release();
		
		//TODO: turn this into a StopDragging method
		DraggingObject = nullptr;
		PlaceablePreview->SetActorHiddenInGame(true);
		bIsDragging = false;	
		
		return;
	}
	
	if (!CurrentHoveredGrid->CanAddPlaceableToGrid(DraggingObject, PlaceablePreview->GetActorLocation(), DraggingObject->GetLocalYaw()))
	{		
		return;
	}

	CurrentHoveredGrid->AddPlaceableToGrid(DraggingObject, PlaceablePreview->GetActorLocation(), DraggingObject->GetLocalYaw());
	
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
