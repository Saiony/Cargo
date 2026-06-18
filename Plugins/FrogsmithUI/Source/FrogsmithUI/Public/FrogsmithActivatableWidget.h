// Copyright Frogsmith. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "FrogsmithActivatableWidget.generated.h"

UENUM(BlueprintType)
enum class EFrogsmithWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

UCLASS()
class FROGSMITHUI_API UFrogsmithActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFrogsmithActivatableWidget(const FObjectInitializer& ObjectInitializer);

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EFrogsmithWidgetInputMode InputConfig = EFrogsmithWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
	
	/** The desired mouse visibility. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	bool bShowMouseCursor = false;

	UPROPERTY()
	EFrogsmithWidgetInputMode PreviousInputMode;
};
