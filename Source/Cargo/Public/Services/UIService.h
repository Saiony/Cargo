// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FORGServiceBase.h"
#include "Templates/Function.h"
#include "UI/Generic/CargoInputTextWidget.h"
#include "UIService.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CARGO_API UUIService : public UFORGServiceBase
{
	GENERATED_BODY()

	FTimerHandle FadeTimer;
	
public:	
	void FadeIn(float Duration, TFunction<void()> Callback);
	void FadeOut(float Duration, TFunction<void()> Callback);

	UUIService();
	
	virtual void Boot(FOnServiceBooted OnBootFinished) override;
	
	UFrogsmithActivatableWidget* ShowWidget(TSubclassOf<UFrogsmithActivatableWidget> WidgetClass) const;

	template<typename T>
	T* ShowWidget(TSubclassOf<T> WidgetClass) const
	{
		return Cast<T>(ShowWidget(TSubclassOf<UFrogsmithActivatableWidget>(WidgetClass)));
	}
	
	UCargoInputTextWidget* ShowInputTextWidget() const;
};
