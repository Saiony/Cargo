// Fill out your copyright notice in the Description page of Project Settings.

#include "GridComponentDADetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "ScopedTransaction.h"
#include "DataAssets/GridComponentDA.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "GridComponentDADetails"

TSharedRef<IDetailCustomization> FGridComponentDADetails::MakeInstance()
{
	return MakeShareable(new FGridComponentDADetails);
}

void FGridComponentDADetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CachedDetailBuilder = &DetailBuilder;

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	// Por simplicidade, só desenhamos o grid quando exatamente 1 asset está selecionado.
	if (ObjectsBeingCustomized.Num() != 1)
	{
		return;
	}

	EditedAsset = Cast<UGridComponentDA>(ObjectsBeingCustomized[0].Get());
	if (!EditedAsset.IsValid())
	{
		return;
	}

	// Ouve mudanças em GridSize para reconstruir o grid quando ele mudar.
	// GridSize é uma struct (FIntVector) editada como campos X/Y/Z separados,
	// então precisamos ouvir tanto o handle "pai" quanto os filhos (X, Y, Z).
	TSharedRef<IPropertyHandle> GridSizeHandle =
		DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UGridComponentDA, GridSize));
	GridSizeHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FGridComponentDADetails::OnGridSizeChanged));
	GridSizeHandle->SetOnChildPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FGridComponentDADetails::OnGridSizeChanged));

	IDetailCategoryBuilder& GridCategory =
		DetailBuilder.EditCategory("Grid Preview", LOCTEXT("GridPreviewCategory", "Grid Preview"), ECategoryPriority::Important);

	GridCategory.AddCustomRow(LOCTEXT("GridPreviewRowFilter", "Grid Preview"))
	.WholeRowContent()
	[
		BuildGridWidget()
	];
}

TSharedRef<SWidget> FGridComponentDADetails::BuildGridWidget()
{
    if (!EditedAsset.IsValid())
    {
        return SNullWidget::NullWidget;
    }

    const int32 SizeX = FMath::Max(EditedAsset->GridSize.X, 0);
    const int32 SizeY = FMath::Max(EditedAsset->GridSize.Y, 0);

    if (SizeX <= 0 || SizeY <= 0)
    {
        return SNew(STextBlock)
            .Text(LOCTEXT(
                "InvalidGridSize",
                "Defina GridSize.X e GridSize.Y maiores que zero para ver o grid."
            ));
    }

    TSharedRef<SUniformGridPanel> GridPanel =
        SNew(SUniformGridPanel)
        .SlotPadding(FMargin(1.0f));

    constexpr float CellSize = 20.0f;

    const int32 MinX = -SizeX / 2;
    const int32 MinY = -SizeY / 2;

    for (int32 X = 0; X < SizeX; ++X)
    {
        for (int32 Y = 0; Y < SizeY; ++Y)
        {
            const int32 GridX = MinX + X;
            const int32 GridY = MinY + Y;

            // Unreal top-down convention:
            // +X = forward = screen up
            // +Y = right   = screen right
            const int32 VisualColumn = Y;
            const int32 VisualRow = SizeX - 1 - X;

            GridPanel->AddSlot(VisualColumn, VisualRow)
            [
                SNew(SBox)
                .WidthOverride(CellSize)
                .HeightOverride(CellSize)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(
                        this,
                        &FGridComponentDADetails::GetCellColor,
                        GridX,
                        GridY
                    )
                    .OnClicked(
                        this,
                        &FGridComponentDADetails::OnCellClicked,
                        GridX,
                        GridY
                    )
                    .ToolTipText(
                        FText::Format(
                            LOCTEXT("CellTooltip", "({0}, {1})"),
                            FText::AsNumber(GridX),
                            FText::AsNumber(GridY)
                        )
                    )
                    .ContentPadding(FMargin(0.0f))
                ]
            ];
        }
    }

    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Left)
        [
            SNew(SBorder)
            .Padding(FMargin(4.0f))
            [
                GridPanel
            ]
        ];
}

bool FGridComponentDADetails::IsCellBlocked(int32 X, int32 Y) const
{
	if (!EditedAsset.IsValid())
	{
		return false;
	}

	const FIntVector Cell(X, Y, 0);
	return EditedAsset->InvalidSlots.Cells.Contains(Cell);
}

FSlateColor FGridComponentDADetails::GetCellColor(int32 X, int32 Y) const
{
	static const FLinearColor BlockedColor(0.35f, 0.35f, 0.35f);
	static const FLinearColor FreeColor(0.1f, 0.4f, 0.9f);

	return IsCellBlocked(X, Y) ? FSlateColor(BlockedColor) : FSlateColor(FreeColor);
}

FReply FGridComponentDADetails::OnCellClicked(int32 X, int32 Y)
{
	if (!EditedAsset.IsValid())
	{
		return FReply::Handled();
	}

	const FScopedTransaction Transaction(LOCTEXT("ToggleBlockedCell", "Alternar célula bloqueada"));
	EditedAsset->Modify();

	const FIntVector Cell(X, Y, 0);
	TArray<FIntVector>& Cells = EditedAsset->InvalidSlots.Cells;

	const int32 ExistingIndex = Cells.IndexOfByKey(Cell);
	if (ExistingIndex != INDEX_NONE)
	{
		Cells.RemoveAt(ExistingIndex);
	}
	else
	{
		Cells.Add(Cell);
	}

	EditedAsset->MarkPackageDirty();

	// Não precisa de ForceRefreshDetails aqui: a cor do botão é um TAttribute
	// que já reconsulta IsCellBlocked a cada paint.
	return FReply::Handled();
}

void FGridComponentDADetails::OnGridSizeChanged()
{
	if (CachedDetailBuilder)
	{
		CachedDetailBuilder->ForceRefreshDetails();
	}
}

#undef LOCTEXT_NAMESPACE