// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FORGServiceBase.generated.h"

DECLARE_DELEGATE(FOnServiceBooted);


UCLASS(DefaultToInstanced)
class CARGO_API UFORGServiceBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UFORGServiceBase()
	{
		PrimaryComponentTick.bCanEverTick = true;
		SetIsReplicatedByDefault(false);
	}
	
	virtual void Boot(FOnServiceBooted OnBootFinished)
	{
		OnBootFinished.ExecuteIfBound();
	}

	virtual void SetReplication(const bool NewStatus);
	virtual bool GetComponentClassCanReplicate() const override { return true; }

protected:
	virtual bool CanEverTick() const { return false; }
};
