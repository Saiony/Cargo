// Copyright Frogsmith. All Rights Reserved.

#include "PrimaryGameLayout.h"


#include "CommonLocalPlayer.h"
#include "Engine/GameInstance.h"
#include "GameUIManagerSubsystem.h"
#include "GameUIPolicy.h"
#include "FrogsmithUI/LogFrogsmithUI.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrimaryGameLayout)

class UObject;

/*static*/
UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!GameInstance)
	{
		return nullptr;
	}
	APlayerController* PlayerController = GameInstance->GetPrimaryPlayerController(false);
	return GetPrimaryGameLayout(PlayerController);
}

/*static*/
UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(APlayerController* PlayerController)
{
	return PlayerController ? GetPrimaryGameLayout(Cast<UCommonLocalPlayer>(PlayerController->Player)) : nullptr;
}

/*static*/
UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(ULocalPlayer* LocalPlayer)
{
	if(!LocalPlayer)
	{
		return nullptr;
	}

	const UCommonLocalPlayer* CommonLocalPlayer = CastChecked<UCommonLocalPlayer>(LocalPlayer);
	const UGameInstance* GameInstance = CommonLocalPlayer->GetGameInstance();
	if(!GameInstance)
	{
		return nullptr;
	}

	UGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UGameUIManagerSubsystem>();
	if(!UIManager)
	{
		return nullptr;
	}

	const UGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy();
	if(!Policy)
	{
		return nullptr;
	}

	if(UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CommonLocalPlayer))
	{
		return RootLayout;
	}

	return nullptr;
}

UPrimaryGameLayout::UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPrimaryGameLayout::SetIsDormant(bool InDormant)
{
	if(bIsDormant != InDormant)
	{
		const ULocalPlayer* LP = GetOwningLocalPlayer();
		const int32 PlayerId = LP ? LP->GetControllerId() : -1;
		const TCHAR* OldDormancyStr = bIsDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* NewDormancyStr = InDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* PrimaryPlayerStr = LP && LP->IsPrimaryPlayer() ? TEXT("[Primary]") : TEXT("[Non-Primary]");
		UE_LOG(LogFrogsmithUI, Display, TEXT("%s PrimaryGameLayout Dormancy changed for [%d] from [%s] to [%s]"), PrimaryPlayerStr, PlayerId,
		       OldDormancyStr, NewDormancyStr);

		bIsDormant = InDormant;
		OnIsDormantChanged();
	}
}

bool UPrimaryGameLayout::HasWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass, FGameplayTag Layer) const
{
	if(Layer != FGameplayTag() && Layers.Contains(Layer))
	{
		return Layers[Layer]->GetWidgetList().ContainsByPredicate([&](const UCommonActivatableWidget* InWidget) { return InWidget->GetClass() == WidgetClass; });
	}

	// Greedy search, if not useful, remove
	for(const auto& LayerKVP : Layers)
	{
		if(LayerKVP.Value->GetWidgetList().ContainsByPredicate([&](const UCommonActivatableWidget* InWidget) { return InWidget->GetClass() == WidgetClass; }))
		{
			return true;
		}
	}
	
	return false;
}

void UPrimaryGameLayout::OnIsDormantChanged()
{
	//@TODO NDarnell Determine what to do with dormancy, in the past we treated dormancy as a way to shutoff rendering
	//and the view for the other local players when we force multiple players to use the player view of a single player.

	//if (UCommonLocalPlayer* LocalPlayer = GetOwningLocalPlayer<UCommonLocalPlayer>())
	//{
	//	// When the root layout is dormant, we don't want to render anything from the owner's view either
	//	LocalPlayer->SetIsPlayerViewEnabled(!bIsDormant);
	//}

	//SetVisibility(bIsDormant ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);

	//OnLayoutDormancyChanged().Broadcast(bIsDormant);
}

void UPrimaryGameLayout::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if(!IsDesignTime())
	{
		LayerWidget->OnTransitioningChanged.AddUObject(this, &UPrimaryGameLayout::OnWidgetStackTransitioning);
		// TODO: Consider allowing a transition duration, we currently set it to 0, because if it's not 0, the
		//       transition effect will cause focus to not transition properly to the new widgets when using
		//       gamepad always.
		LayerWidget->SetTransitionDuration(0.0);

		Layers.Add(LayerTag, LayerWidget);
	}
}

void UPrimaryGameLayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning)
{
	if(bIsTransitioning)
	{
		const FName SuspendToken = UCommonUIExtensions::SuspendInputForPlayer(GetOwningLocalPlayer(), TEXT("GlobalStackTransion"));
		SuspendInputTokens.Add(SuspendToken);
		return;
	}

	if(ensure(SuspendInputTokens.Num() > 0))
	{
		const FName SuspendToken = SuspendInputTokens.Pop();
		UCommonUIExtensions::ResumeInputForPlayer(GetOwningLocalPlayer(), SuspendToken);
	}
}

void UPrimaryGameLayout::FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget)
{
	// We're not sure what layer the widget is on so go searching.
	for(const auto& LayerKVP : Layers)
	{
		LayerKVP.Value->RemoveWidget(*ActivatableWidget);
	}
}

UCommonActivatableWidgetContainerBase* UPrimaryGameLayout::GetLayerWidget(FGameplayTag LayerName)
{
	return Layers.FindRef(LayerName);
}
