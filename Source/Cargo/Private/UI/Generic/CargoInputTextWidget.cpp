#include "UI/Generic/CargoInputTextWidget.h"

#include "PrimaryGameLayout.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UCargoInputTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	InputTextBox->OnTextChanged.AddDynamic(this, &ThisClass::OnTextChanged);
	ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::OnConfirmButtonClicked);
}

void UCargoInputTextWidget::Init(const FString& Title, const FString& PreviewText, const TScriptInterface<ICargoInputTextListener> InListener)
{
	TitleText->SetText(FText::FromString(Title));
	InputTextBox->SetHintText(FText::FromString(PreviewText));
	Listener = InListener;	
}

void UCargoInputTextWidget::Hide()
{
	auto Layout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this);
	Layout->FindAndRemoveWidgetFromLayer(this);
	
	RemoveFromParent();
}

void UCargoInputTextWidget::OnTextChanged(const FText& Text)
{
	PlaySound(TypingSound);
}

void UCargoInputTextWidget::OnConfirmButtonClicked()
{
	Listener->OnCargoInputTextConfirmed(InputTextBox->GetText().ToString());
}

