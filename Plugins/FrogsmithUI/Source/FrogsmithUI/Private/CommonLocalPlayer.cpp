// Copyright Frogsmith. All Rights Reserved.

#include "CommonLocalPlayer.h"

#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "GameUIManagerSubsystem.h"
#include "GameUIPolicy.h"


UCommonLocalPlayer::UCommonLocalPlayer()
	:Super(FObjectInitializer::Get())
{
}

FDelegateHandle UCommonLocalPlayer::CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate)
{

	if (APlayerController* PC = GetPlayerController(GetWorld()))
	{
		Delegate.Execute(this, PC);
	}

	return OnPlayerControllerSet.Add(Delegate);
}

FDelegateHandle UCommonLocalPlayer::CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate)
{
	const APlayerController* PC = GetPlayerController(GetWorld());
	if (APlayerState* PlayerState = PC ? PC->PlayerState : nullptr)
	{
		Delegate.Execute(this, PlayerState);
	}
	
	return OnPlayerStateSet.Add(Delegate);
}

FDelegateHandle UCommonLocalPlayer::CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate)
{
	const APlayerController* PC = GetPlayerController(GetWorld());
	if (APawn* Pawn = PC ? PC->GetPawn() : nullptr)
	{
		Delegate.Execute(this, Pawn);
	}

	return OnPlayerPawnSet.Add(Delegate);
}

bool UCommonLocalPlayer::GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData, int32 StereoViewIndex) const
{
	if (!bIsPlayerViewEnabled)
	{
		return false;
	}

	return Super::GetProjectionData(Viewport, ProjectionData, StereoViewIndex);
}

UPrimaryGameLayout* UCommonLocalPlayer::GetRootUILayout() const
{
	UGameUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UGameUIManagerSubsystem>();
	if (!UIManager)
	{
		return nullptr;
	}

	if (const UGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy())
	{
		return Policy->GetRootLayout(this);
	}

	return nullptr;
}