// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DIalogueWidget.h"

#include "CommonTextBlock.h"
#include "CommonUIExtensions.h"
#include "PrimaryGameLayout.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Input/CommonUIInputTypes.h"
#include "Kismet/GameplayStatics.h"

void UDIalogueWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	CurrentLineIndex = -1;
	bIsTyping = false;
	CurrentCharCount = 0.0f;
	FullLineText = FText::GetEmpty();
	DialogueDefinition = nullptr;
	
	bIsBackHandler = false;
	if(!SkipDialogueInputHandle.IsValid())
	{
		constexpr bool bShouldDisplayInActionBar = false;
		SkipDialogueInputHandle = RegisterUIActionBinding(FBindUIActionArgs(SkipDialogueInputActionData,
		                                                                    bShouldDisplayInActionBar,
		                                                                    FSimpleDelegate::CreateUObject(
			                                                                    this, &UDIalogueWidget::OnInputActionContinue)));
	}
}

void UDIalogueWidget::NativeOnDeactivated()
{
	UE_LOG(LogTemp, Warning, TEXT("UARCDialogueWidget::NativeOnDeactivated called on instance %p"), this);
	
	if (SkipDialogueInputHandle.IsValid())
	{
		SkipDialogueInputHandle.Unregister();
	}
	
	Super::NativeOnDeactivated();
}

void UDIalogueWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	UE_LOG(LogTemp, Warning, TEXT("OnAnimationFinished called on instance %p with Animation %p (ShowAnimation: %p, HideAnimation: %p)"), 
		this, Animation, ShowAnimation.Get(), HideAnimation.Get());
	if(Animation == ShowAnimation)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowAnimation finished, calling SetupAndPlayDialogue"));
		SetupAndPlayDialogue();
		return;
	}

	if(Animation == HideAnimation)
	{
		UE_LOG(LogTemp, Warning, TEXT("HideAnimation finished, popping widget"));
		UCommonUIExtensions::PopContentFromLayer(this);
	}
}

void UDIalogueWidget::SetupAndPlayDialogue()
{
	UE_LOG(LogTemp, Warning, TEXT("SetupAndPlayDialogue called on instance %p"), this);

	if (!DialogueDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("SetupAndPlayDialogue: DialogueDefinition is null — closing widget."));
		RequestClose();
		return;
	}

	CurrentLineIndex = -1;
	UE_LOG(LogTemp, Warning, TEXT("About to call ShowNextLine, CurrentLineIndex: %d, DialogueDefinition: %p"), 
		CurrentLineIndex, DialogueDefinition.Get());
	ShowNextLine();
	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine returned"));
}

void UDIalogueWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!bIsTyping || !TextDialogue || !DialogueDefinition)
	{
		return;
	}

	CurrentCharCount += (TypewriterSpeed * InDeltaTime);
	const int32 CharsToShow = FMath::FloorToInt(CurrentCharCount);
	if(CharsToShow >= FullLineText.ToString().Len())
	{
		TextDialogue->SetText(FullLineText);
		bIsTyping = false;
	}
	else
	{
		const FString PartialText = FullLineText.ToString().Left(CharsToShow);
		TextDialogue->SetText(FText::FromString(PartialText));
		if (DialogueAudio)
		{
			UGameplayStatics::PlaySound2D(this, DialogueAudio, 1.0f, FMath::FRandRange(0.9f, 1.1f), 0, DialogueConcurrency);
		}
	}
}

void UDIalogueWidget::ShowNextLine()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine called on instance %p"), this);
	
 	if(!DialogueDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("ShowNextLine: DialogueDefinition is null!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: DialogueDefinition is valid: %s"), *DialogueDefinition->GetName());
	CurrentLineIndex++;
	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: CurrentLineIndex incremented to %d"), CurrentLineIndex);

	if(!DialogueDefinition->DialogueLines.IsValidIndex(CurrentLineIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: No more lines (CurrentLineIndex: %d, Array Num: %d), calling RequestClose"), 
			CurrentLineIndex, DialogueDefinition->DialogueLines.Num());
		RequestClose();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: Getting line at index %d"), CurrentLineIndex);
	const FARCDialogueLine& Line = DialogueDefinition->DialogueLines[CurrentLineIndex];

	FullLineText = Line.Text;
	TextDialogue->SetText(FText::GetEmpty());
	CurrentCharCount = 0.0f;
	bIsTyping = true;

	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: Line text set, calling UpdateVisualsForLine"));
	UpdateVisualsForLine(Line);
	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: Complete"));
}

void UDIalogueWidget::UpdateVisualsForLine(const FARCDialogueLine& Line)
{
	bool bShouldUpdatePortraits = false;
	if(Line.PortraitSide == Left && ImageLeftPortrait)
	{
		ImageRightPortrait->SetVisibility(ESlateVisibility::Hidden);
		ImageLeftPortrait->SetVisibility(ESlateVisibility::Visible);

		if(Line.bOverridePortrait)
		{
			bShouldUpdatePortraits = ImageLeftPortrait->GetBrush() != Line.PortraitOverride;
			ImageLeftPortrait->SetBrush(Line.PortraitOverride);
		}
		else
		{
			bShouldUpdatePortraits = ImageLeftPortrait->GetBrush() != DialogueDefinition->DefaultPortrait;
			ImageLeftPortrait->SetBrush(DialogueDefinition->DefaultPortrait);
		}
	}
	else if(Line.PortraitSide == Right && ImageRightPortrait)
	{
		ImageRightPortrait->SetVisibility(ESlateVisibility::Visible);
		ImageLeftPortrait->SetVisibility(ESlateVisibility::Hidden);

		if(Line.bOverridePortrait)
		{
			bShouldUpdatePortraits = ImageRightPortrait->GetBrush() != Line.PortraitOverride;
			ImageRightPortrait->SetBrush(Line.PortraitOverride);
		}
		else
		{
			bShouldUpdatePortraits = ImageRightPortrait->GetBrush() != DialogueDefinition->DefaultPortrait;
			ImageRightPortrait->SetBrush(DialogueDefinition->DefaultPortrait);
		}
	}

	if (bShouldUpdatePortraits && LineTransitionAnimation && CurrentLineIndex > 0)
		PlayAnimation(LineTransitionAnimation);
}

void UDIalogueWidget::InitializeDialogue(UDialogueData* InDialogueDefinition)
{
	DialogueDefinition = InDialogueDefinition;
	UE_LOG(LogTemp, Warning, TEXT("InitializeDialogue called with definition: %s"), InDialogueDefinition ? *InDialogueDefinition->GetName() : TEXT("nullptr"));
	
	if(ShowAnimation)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowAnimation exists, playing it. Animation pointer: %p, Duration: %f"), 
			ShowAnimation.Get(), ShowAnimation ? ShowAnimation->GetEndTime() : 0.0f);
		TextDialogue->SetText(FText::GetEmpty());
		if(InDialogueDefinition->DialogueLines[0].PortraitSide == Left)
		{
			ImageLeftPortrait->SetBrush(InDialogueDefinition->DefaultPortrait);
			ImageLeftPortrait->SetVisibility(ESlateVisibility::Visible);
			ImageRightPortrait->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			ImageRightPortrait->SetBrush(InDialogueDefinition->DefaultPortrait);
			ImageLeftPortrait->SetVisibility(ESlateVisibility::Hidden);
			ImageRightPortrait->SetVisibility(ESlateVisibility::Visible);
		}
		PlayAnimation(ShowAnimation);
		UE_LOG(LogTemp, Warning, TEXT("PlayAnimation called for ShowAnimation"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ShowAnimation does not exist, calling SetupAndPlayDialogue directly"));
	SetupAndPlayDialogue();
}

void UDIalogueWidget::OnInputActionContinue()
{
	if(bIsTyping)
	{
		TextDialogue->SetText(FullLineText);
		bIsTyping = false;
		return;
	}

	ShowNextLine();
}

void UDIalogueWidget::RequestClose()
{
	if(HideAnimation)
	{
		PlayAnimation(HideAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, true);
		return;
	}

	UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this)->FindAndRemoveWidgetFromLayer(this);
}
