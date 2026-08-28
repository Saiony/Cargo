// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Input/Reply.h"
#include "Styling/SlateColor.h"

class UGridComponentDA;
class IDetailLayoutBuilder;
class SWidget;


class FGridComponentDADetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TWeakObjectPtr<UGridComponentDA> EditedAsset;

	IDetailLayoutBuilder* CachedDetailBuilder = nullptr;

	TSharedRef<SWidget> BuildGridWidget();

	bool IsCellBlocked(int32 X, int32 Y) const;

	FSlateColor GetCellColor(int32 X, int32 Y) const;

	FReply OnCellClicked(int32 X, int32 Y);

	void OnGridSizeChanged();
};