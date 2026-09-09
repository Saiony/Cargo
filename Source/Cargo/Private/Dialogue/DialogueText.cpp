#include "Dialogue/DialogueText.h"

#include "Internationalization/Regex.h"

void FDialogueText::Parse(const FString& Text, TFunctionRef<TOptional<FString>(const FString&)> ResolvePlayerData)
{
	PlainText.Reset();
	Segments.Reset();
	const FRegexPattern Pattern(TEXT("\\[\\[(PlayerData\\.[A-Za-z0-9_.]+)\\]\\]"));
	FRegexMatcher Matcher(Pattern, Text);
	int32 Offset = 0;
	while (Matcher.FindNext())
	{
		Segments.Add({Text.Mid(Offset, Matcher.GetMatchBeginning() - Offset), false});
		const auto Value = ResolvePlayerData(Matcher.GetCaptureGroup(1));
		const FString Token = Text.Mid(Matcher.GetMatchBeginning(), Matcher.GetMatchEnding() - Matcher.GetMatchBeginning());
		Segments.Add({Value.IsSet() ? Value.GetValue() : Token, Value.IsSet()});
		Offset = Matcher.GetMatchEnding();
	}
	Segments.Add({Text.Mid(Offset), false});
	for (const auto& Segment : Segments)
	{
		PlainText += Segment.Text;
	}
}

FText FDialogueText::ToRichText(int32 VisibleCharacters) const
{
	FString Result;
	for (const auto& Segment : Segments)
	{
		const int32 Count = FMath::Clamp(VisibleCharacters, 0, Segment.Text.Len());
		if (Count == 0)
		{
			continue;
		}
		FString VisibleText = Segment.Text.Left(Count);
		VisibleText.ReplaceInline(TEXT("&"), TEXT("&amp;"));
		VisibleText.ReplaceInline(TEXT("<"), TEXT("&lt;"));
		VisibleText.ReplaceInline(TEXT(">"), TEXT("&gt;"));
		VisibleText.ReplaceInline(TEXT("\""), TEXT("&quot;"));
		Result += Segment.bPlayerData ? TEXT("<PlayerData>") + VisibleText + TEXT("</>") : VisibleText;
		VisibleCharacters -= Count;
	}
	return FText::FromString(Result);
}
