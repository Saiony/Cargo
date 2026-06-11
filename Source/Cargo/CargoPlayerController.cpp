// Copyright Epic Games, Inc. All Rights Reserved.


#include "CargoPlayerController.h"
#include "CargoCharacter.h"
#include "Public/Grid/Placeable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Cargo.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ACargoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogCargo, Error, TEXT("Could not spawn mobile controls widget."));
		}

	}
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

	if (!bEditMode)
		return;
	
	if (bIsDragging && DraggingObject)
	{
		FVector MouseWorldLocation, MouseWorldDirection;
		if (DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
		{
			// Projetar para o plano Z do DraggingZHeight
			// P = L + D*t -> Pz = Lz + Dz*t -> t = (Pz - Lz) / Dz
			float t = (DraggingZHeight - MouseWorldLocation.Z) / MouseWorldDirection.Z;
			FVector TargetLocation = MouseWorldLocation + MouseWorldDirection * t;
			
			DraggingObject->SetActorLocation(TargetLocation);
		}
	}
}

void ACargoPlayerController::OnLeftClickStart(const FInputActionValue& Value)
{
	FHitResult HitResult;
	
	if (!bEditMode)
		return;
	
	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		if (APlaceable* Placeable = Cast<APlaceable>(HitResult.GetActor()))
		{
			DraggingObject = Placeable;
			bIsDragging = true;
			
			// Se o objeto estiver anexado a algo, desanexar ao começar a arrastar
			DraggingObject->Grab();
			
			// Opcional: Desativar física se necessário enquanto arrasta
			//DraggingObject->DisableComponentsSimulatePhysics();
		}
	}
}

void ACargoPlayerController::OnLeftClickEnd(const FInputActionValue& InputActionValue)
{
	if (!bEditMode)
		return;
	
	if (bIsDragging && DraggingObject)
	{
		FHitResult HitResult;		
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(DraggingObject);
		
		FVector TraceStart = DraggingObject->GetActorLocation();
		FVector TraceEnd   = FVector(TraceStart.X, TraceStart.Y, TraceStart.Z - 10000.f);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			if (ACargoCharacter* CharacterHit = Cast<ACargoCharacter>(HitResult.GetActor()))
			{
				CharacterHit->AddPlaceableToGrid(DraggingObject, HitResult.ImpactPoint);
			}
		}		
		
		bIsDragging = false;
		DraggingObject = nullptr;
	}
}

void ACargoPlayerController::OnRightClick(const FInputActionValue& Value)
{
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

	if (APawn* ControlledPawn = GetPawn())
	{
		SetViewTargetWithBlend(ControlledPawn, 0.5f);
	}
}
