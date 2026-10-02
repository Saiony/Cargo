#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestData.h"
#include "QuestCollection.generated.h"

USTRUCT(BlueprintType)
struct CARGO_API FQuestCollection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Quest")
	TArray<TObjectPtr<UQuestData>> Quests;
};
