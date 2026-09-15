#include "UI/DialogueOptionButton.h"

#include "CommonTextBlock.h"
#include "Subsystem/FROGDialogueSubsystem.h"

void UDialogueOptionButton::Init(const FText& Title, const int8 InId, IDialogueOptionListener* InListener)
{
	Text->SetText(Title);
	Id = InId;
	Listener = InListener;
	Dialogue = nullptr;
	Instigator = nullptr;
}

void UDialogueOptionButton::Init(const FText& Title, UDialogueData* InDialogue, AActor* InInstigator, IDialogueOptionListener* InListener)
{
	Init(Title, -1, InListener);
	Dialogue = InDialogue;
	Instigator = InInstigator;
}

void UDialogueOptionButton::NativeOnClicked()
{
	Super::NativeOnClicked();
	if (Dialogue)
		GetGameInstance()->GetSubsystem<UFROGDialogueSubsystem>()->PlayDialogue(Dialogue, Instigator);
	Listener->OnDialogueOptionClicked(this, Id);
}
