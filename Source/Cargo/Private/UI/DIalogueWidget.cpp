#include "UI/DIalogueWidget.h"

#include "CargoGameMode.h"
#include "Subsystem/FROGDialogueSubsystem.h"
#include "Components/RichTextBlock.h"
#include "DeveloperSettings/CargoSettings.h"
#include "GameplayFramework/CargoPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "CommonUIExtensions.h"
#include "PrimaryGameLayout.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/VerticalBoxSlot.h"
#include "Input/CommonUIInputTypes.h"
#include "Kismet/GameplayStatics.h"

void UDIalogueWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	CurrentLineIndex = -1;
	bIsTyping = false;
	CurrentCharCount = 0.0f;
	FullLineText = FText::GetEmpty();
	CurrentDialogueData = nullptr;
	
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
		FinishHide();
	}
}

void UDIalogueWidget::SetupAndPlayDialogue()
{
	UE_LOG(LogTemp, Warning, TEXT("SetupAndPlayDialogue called on instance %p"), this);

	if (!CurrentDialogueData)
	{
		UE_LOG(LogTemp, Error, TEXT("SetupAndPlayDialogue: CurrentDialogueData is null — closing widget."));
		Hide();
		return;
	}

	CurrentLineIndex = -1;
	UE_LOG(LogTemp, Warning, TEXT("About to call ShowNextLine, CurrentLineIndex: %d, CurrentDialogueData: %p"), 
		CurrentLineIndex, CurrentDialogueData.Get());
	ShowNextLine();
	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine returned"));
}

void UDIalogueWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!bIsTyping || !TextDialogue || !CurrentDialogueData)
	{
		return;
	}

	CurrentCharCount += (TypewriterSpeed * InDeltaTime);
	const int32 CharsToShow = FMath::FloorToInt(CurrentCharCount);
	const FString FullStr = FullLineText.ToString();

	if(CharsToShow >= FullStr.Len())
	{
		TextDialogue->SetText(FormattedLine.ToRichText(FormattedLine.PlainText.Len()));
		bIsTyping = false;
		return;
	}
	if (CharsToShow > LastCharsShown)
	{
		TextDialogue->SetText(FormattedLine.ToRichText(CharsToShow));
		for (int32 i = LastCharsShown; i < CharsToShow; ++i)
		{
			const TCHAR Ch = FullStr[i];
			const bool bShouldPlaySound = !FChar::IsWhitespace(Ch) && (i % FMath::Max(1, CharsPerSound) == 0);

			if (bShouldPlaySound && DialogueAudio)
			{
				UGameplayStatics::PlaySound2D(this, DialogueAudio, 1.0f, FMath::FRandRange(0.9f, 1.1f), 0, DialogueConcurrency);
			}
		}

		LastCharsShown = CharsToShow;
	}
}

void UDIalogueWidget::ShowNextLine()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine called on instance %p"), this);
	
 	if(!CurrentDialogueData)
	{
		UE_LOG(LogTemp, Error, TEXT("ShowNextLine: CurrentDialogueData is null!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: CurrentDialogueData is valid: %s"), *CurrentDialogueData->GetName());
	CurrentLineIndex++;
	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: CurrentLineIndex incremented to %d"), CurrentLineIndex);

	if(!CurrentDialogueData->DialogueLines.IsValidIndex(CurrentLineIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: No more lines (CurrentLineIndex: %d, Array Num: %d), calling Hide"), 
			CurrentLineIndex, CurrentDialogueData->DialogueLines.Num());
		OnDialogueFinished();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ShowNextLine: Getting line at index %d"), CurrentLineIndex);
	const FARCDialogueLine& Line = CurrentDialogueData->DialogueLines[CurrentLineIndex];

	PrepareLineText(Line.Text);
	TextDialogue->SetText(FText::GetEmpty());
	CurrentCharCount = 0.0f;
	LastCharsShown = 0;
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
			bShouldUpdatePortraits = ImageLeftPortrait->GetBrush() != CurrentDialogueData->DefaultPortrait;
			ImageLeftPortrait->SetBrush(CurrentDialogueData->DefaultPortrait);
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
			bShouldUpdatePortraits = ImageRightPortrait->GetBrush() != CurrentDialogueData->DefaultPortrait;
			ImageRightPortrait->SetBrush(CurrentDialogueData->DefaultPortrait);
		}
	}

	if (bShouldUpdatePortraits && LineTransitionAnimation && CurrentLineIndex > 0)
		PlayAnimation(LineTransitionAnimation);
}

void UDIalogueWidget::OnDialogueFinished()
{
	if (CurrentDialogueData->Choices.IsEmpty())
	{		
		Hide();
		return;
	}
	
	DisplayChoices();
}

void UDIalogueWidget::DisplayChoices()
{
	for (auto i = 0; i < CurrentDialogueData->Choices.Num(); i++)
	{
		const auto OptionButton = CreateWidget<UDialogueOptionButton>(this, DialogueOptionButtonClass);
		OptionButton->Init(CurrentDialogueData->Choices[i]);
		OptionButton->OnClicked().AddUObject(this, &ThisClass::OnChoiceSelected, i);
		
		const auto ChildrenSlot = OptionsVerticalBox->AddChildToVerticalBox(OptionButton);
		ChildrenSlot->SetPadding(FMargin(0.f, 0.f, 0.f, ChildrenPadding)); 
	}
}

void UDIalogueWidget::OnChoiceSelected(int buttonIndex)
{
	OptionsVerticalBox->ClearChildren();
	
	const auto& SelectedChoice = CurrentDialogueData->Choices[buttonIndex];
	ACargoGameMode::Get(this)->AddTag(SelectedChoice.ChoiceTag);
	GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>()->SetNextDialogue(SelectedChoice.DialogueData);

	Hide();
}

void UDIalogueWidget::InitializeDialogue(UDialogueData* InDialogueDefinition)
{
	if (InDialogueDefinition == nullptr)
	{		
		UE_LOG(LogTemp, Error, TEXT("Probably we need to call .LoadSynchronous if there's a dialoguedata there"));
		return;
	}
	
	CurrentDialogueData = InDialogueDefinition;
	UE_LOG(LogTemp, Log, TEXT("InitializeDialogue called with definition: %s"), InDialogueDefinition ? *InDialogueDefinition->GetName() : TEXT("nullptr"));

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
	if (!CurrentDialogueData || OptionsVerticalBox->GetChildrenCount() > 0 || CurrentLineIndex >= CurrentDialogueData->DialogueLines.Num())
	{
		return;
	}

	if(bIsTyping)
	{
		TextDialogue->SetText(FormattedLine.ToRichText(FormattedLine.PlainText.Len()));
		bIsTyping = false;
		return;
	}

	ShowNextLine();
}

void UDIalogueWidget::Hide()
{
	if(HideAnimation)
	{
		PlayAnimation(HideAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, true);
		return;
	}

	FinishHide();
}

void UDIalogueWidget::FinishHide()
{
	const auto Definition = CurrentDialogueData;
	const auto Completion = OnDialogueFinishedDelegate;
	OnDialogueFinishedDelegate.Clear();
	UCommonUIExtensions::PopContentFromLayer(this);
	Completion.Broadcast(Definition);
}

void UDIalogueWidget::PrepareLineText(const FText& Text)
{
	const auto PlayerState = GetOwningPlayer()->GetPlayerState<ACargoPlayerState>();
	FormattedLine.Parse(Text.ToString(), [PlayerState](const FString& TagName) -> TOptional<FString>
	{
		const auto Tag = FGameplayTag::RequestGameplayTag(FName(*TagName), false);
		const auto Value = PlayerState ? PlayerState->FindPlayerDataTag(Tag) : nullptr;
		return Value ? TOptional<FString>(*Value) : TOptional<FString>();
	});
	FullLineText = FText::FromString(FormattedLine.PlainText);
	UpdatePlayerDataStyle();
}

void UDIalogueWidget::UpdatePlayerDataStyle()
{
	auto Styles = NewObject<UDataTable>(TextDialogue);
	Styles->RowStruct = FRichTextStyleRow::StaticStruct();
	FRichTextStyleRow Row;
	Row.TextStyle = TextDialogue->GetCurrentDefaultTextStyle();
	Styles->AddRow(TEXT("Default"), Row);
	Row.TextStyle.SetColorAndOpacity(GetDefault<UCargoSettings>()->DialoguePlayerDataColor);
	Styles->AddRow(TEXT("PlayerData"), Row);
	TextDialogue->SetTextStyleSet(Styles);
}
