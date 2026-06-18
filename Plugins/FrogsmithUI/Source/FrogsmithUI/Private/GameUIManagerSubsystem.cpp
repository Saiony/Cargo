// Copyright Frogsmith. All Rights Reserved.

#include "GameUIManagerSubsystem.h"

#include "FrogsmithUISettings.h"
#include "GameUIPolicy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameUIManagerSubsystem)

class UClass;

void UGameUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!CurrentPolicy)
	{
		TSoftClassPtr<UGameUIPolicy> PolicyClassPtr = DefaultUIPolicyClass;

		// Fallback to settings if not set in config
		if (PolicyClassPtr.IsNull())
		{
			if (const UFrogsmithUISettings* Settings = GetDefault<UFrogsmithUISettings>())
			{
				PolicyClassPtr = Settings->DefaultUIPolicyClass;
			}
		}

		if (!PolicyClassPtr.IsNull())
		{
			const TSubclassOf<UGameUIPolicy> PolicyClass = PolicyClassPtr.LoadSynchronous();
			SwitchToPolicy(NewObject<UGameUIPolicy>(this, PolicyClass));
		}
	}
}

void UGameUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	SwitchToPolicy(nullptr);
}

bool UGameUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UGameUIManagerSubsystem::NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer) && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerAdded(LocalPlayer);
	}
}

void UGameUIManagerSubsystem::NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerRemoved(LocalPlayer);
	}
}

void UGameUIManagerSubsystem::NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerDestroyed(LocalPlayer);
	}
}

void UGameUIManagerSubsystem::SwitchToPolicy(UGameUIPolicy* InPolicy)
{
	if (CurrentPolicy != InPolicy)
	{
		CurrentPolicy = InPolicy;
	}
}
