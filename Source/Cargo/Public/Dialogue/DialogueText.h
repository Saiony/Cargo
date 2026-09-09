#pragma once

#include "CoreMinimal.h"

// Stores a text fragment and whether it comes from PlayerData and should use the highlight color.
struct FDialogueTextSegment
{
	FString Text;
	bool bPlayerData = false;
};

// Stores the resolved dialogue text and its fragments to preserve highlighting during the typewriter effect.
struct CARGO_API FDialogueText
{
	FString PlainText;
	TArray<FDialogueTextSegment> Segments;

	// Replaces [[PlayerData.*]] tokens using the resolver and records which fragments should be highlighted.
	void Parse(const FString& Text, TFunctionRef<TOptional<FString>(const FString&)> ResolvePlayerData);
	
	// Returns the requested number of visible characters with complete color tags and escaped literal text.
	FText ToRichText(int32 VisibleCharacters) const;
};
