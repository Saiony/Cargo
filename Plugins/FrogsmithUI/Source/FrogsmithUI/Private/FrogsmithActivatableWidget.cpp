// Copyright Frogsmith. All Rights Reserved.


#include "FrogsmithActivatableWidget.h"

#include "Editor/WidgetCompilerLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FrogsmithActivatableWidget)

#define LOCTEXT_NAMESPACE "FrogsmithUI"

UFrogsmithActivatableWidget::UFrogsmithActivatableWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UFrogsmithActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EFrogsmithWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode, !bShowMouseCursor);
	case EFrogsmithWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode, !bShowMouseCursor);
	case EFrogsmithWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, !bShowMouseCursor);
	case EFrogsmithWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}

void UFrogsmithActivatableWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

#if WITH_EDITOR

void UFrogsmithActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledWidgetTree(BlueprintWidgetTree, CompileLog);

	if (!GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UFrogsmithActivatableWidget, BP_GetDesiredFocusTarget)))
	{
		if (GetParentNativeClass(GetClass()) == UFrogsmithActivatableWidget::StaticClass())
		{
			CompileLog.Warning(LOCTEXT("ValidateGetDesiredFocusTarget_Warning", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen."));
		}
		else
		{
			//TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
			CompileLog.Note(LOCTEXT("ValidateGetDesiredFocusTarget_Note", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.  If it was implemented in the native base class you can ignore this message."));
		}
	}
}

#endif

#undef LOCTEXT_NAMESPACE
