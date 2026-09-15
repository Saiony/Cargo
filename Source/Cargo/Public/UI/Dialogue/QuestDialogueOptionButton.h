#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "QuestDialogueOptionButton.generated.h"

class ACargoIsland;
class UCommonTextBlock;
class UDialogueData;
class UQuestData;
class UQuestDialogueOptionButton;

UINTERFACE()
class UQuestDialogueOptionListener : public UInterface
{
	GENERATED_BODY()
};

class IQuestDialogueOptionListener
{
	GENERATED_BODY()

public:
	virtual void OnQuestDialogueOptionClicked(UQuestDialogueOptionButton* Button) = 0;
};

UENUM(BlueprintType)
enum class EQuestDialogueOptionType : uint8
{
	StartQuest,
	QuestInProgress,
	EndQuest,
};

UCLASS()
class CARGO_API UQuestDialogueOptionButton : public UCommonButtonBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text;

	UPROPERTY()
	TObjectPtr<UQuestData> QuestData;

	UPROPERTY()
	TObjectPtr<ACargoIsland> Island;

	IQuestDialogueOptionListener* Listener = nullptr;

	void PlayDialogue(TSoftObjectPtr<UDialogueData> Dialogue);
	void EndQuest();

protected:
	UPROPERTY(BlueprintReadOnly, Category="Cargo")
	EQuestDialogueOptionType Type;

	virtual void NativeOnClicked() override;

public:
	void Init(UQuestData* InQuestData, EQuestDialogueOptionType InType, ACargoIsland* InIsland, IQuestDialogueOptionListener* InListener);
};
