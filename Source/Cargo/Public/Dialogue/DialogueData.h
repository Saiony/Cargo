// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DialogueData.generated.h"


class UDialogueData;
class UARCDialogueCallbackBase;

UENUM(BlueprintType)
enum EARCDialoguePortraitSide : uint8
{
	Left = 0,
	Right
};

USTRUCT(BlueprintType)
struct FCargoDialogueChoice
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true), Category="Cargo")
	FText Text;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo", meta = (Categories = "Choice"))
	FGameplayTag ChoiceTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	TSoftObjectPtr<UDialogueData> DialogueData; 
};

USTRUCT(BlueprintType)
struct FARCDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true), Category="Arcade")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Arcade")
	TEnumAsByte<EARCDialoguePortraitSide> PortraitSide = Left;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Arcade")
	bool bOverridePortrait = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverridePortrait", EditConditionHides), Category="Arcade")
	FSlateBrush PortraitOverride;
};

USTRUCT(BlueprintType)
struct FDialogueWithCondition
{
	GENERATED_BODY()	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo", meta=(Categories="Choice"))
	FGameplayTagContainer RequiredChoiceTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	TSoftObjectPtr<UDialogueData> AlternativeDialogue;	
};

/**
 * 
 */
UCLASS()
class CARGO_API UDialogueData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** Gameplay tag that uniquely identifies this dialogue. Used by the subsystem to build the runtime registry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Arcade", meta = (Categories = "Dialogue"))
	FGameplayTag DialogueTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Arcade")
	FSlateBrush DefaultPortrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Arcade")
	TArray<FARCDialogueLine> DialogueLines;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cargo")
	TArray<FCargoDialogueChoice> Choices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category="Arcade")
	TArray<UARCDialogueCallbackBase*> PreDialogueCallbacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category="Arcade")
	TArray<UARCDialogueCallbackBase*> PostDialogueCallbacks;

	const FPrimaryAssetType CollectionType = FPrimaryAssetType(TEXT("DIALOGUE_DATA"));
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(CollectionType, GetFName());
	}
};
