// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FORGServiceBase.h"
#include "UI/Generic/CargoInputTextWidget.h"
#include "UIService.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UUIService : public UFORGServiceBase
{
	GENERATED_BODY()
	
public:	
	UUIService();
	
	virtual void Boot(FOnServiceBooted OnBootFinished) override;
	
	UFrogsmithActivatableWidget* ShowWidget(TSubclassOf<UFrogsmithActivatableWidget> WidgetClass) const;
	
	UCargoInputTextWidget* ShowInputTextWidget() const;
};
