#include "PlaceableEditor.h"
#include "Cargo/Public/Grid/Placeable.h"
#include "Cargo/Public/DataAssets/ContainerDA.h"

#include "Components/InstancedStaticMeshComponent.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Settings/CargoEditorSettings.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FPlaceableEditorDetails"

class UCargoEditorSettings;
static TMap<TWeakObjectPtr<APlaceable>, TWeakObjectPtr<UInstancedStaticMeshComponent>> GDebugISMs;

TSharedRef<IDetailCustomization> FPlaceableEditorDetails::MakeInstance()
{
	return MakeShareable(new FPlaceableEditorDetails);
}

void FPlaceableEditorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);

	APlaceable* Placeable = nullptr;
	for (const TWeakObjectPtr<UObject>& Obj : SelectedObjects)
	{
		if (APlaceable* Found = Cast<APlaceable>(Obj.Get()))
		{
			Placeable = Found;
			break;
		}
	}

	if (!Placeable)
	{
		return;
	}

	IDetailCategoryBuilder& CargoCategory = DetailBuilder.EditCategory("Cargo", FText::GetEmpty(), ECategoryPriority::Important);
	CargoCategory.SetSortOrder(0);

	CargoCategory.AddCustomRow(LOCTEXT("RefreshDataRow", "RefreshData"))
	[
		SNew(SButton)
		.Text(LOCTEXT("RefreshData", "Refresh Data"))
		.HAlign(HAlign_Center)
		.OnClicked(this, &FPlaceableEditorDetails::OnRefreshDataClicked, Placeable)
	];
}

FReply FPlaceableEditorDetails::OnRefreshDataClicked(APlaceable* Placeable)
{
	LoadShape(Placeable);
	return FReply::Handled();
}

void FPlaceableEditorDetails::LoadShape(APlaceable* Placeable)
{
	if (!Placeable || !Placeable->PlaceableDA)
	{
		return;
	}

	Placeable->GridShapeDefinition = Placeable->PlaceableDA->Shape;
	RefreshShapeDebugVisualization(Placeable);
}

UInstancedStaticMeshComponent* FPlaceableEditorDetails::GetOrCreateDebugISM(APlaceable* Placeable)
{
	if (TWeakObjectPtr<UInstancedStaticMeshComponent>* Existing = GDebugISMs.Find(Placeable))
	{
		if (Existing->IsValid())
		{
			return Existing->Get();
		}
	}

	if (!Placeable->GetWorld())
	{
		return nullptr;
	}

	UInstancedStaticMeshComponent* NewISM = NewObject<UInstancedStaticMeshComponent>(Placeable, TEXT("Editor_DebugShapeISM"), RF_Transient);
	NewISM->SetupAttachment(Placeable->GetRootComponent());
	NewISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewISM->SetHiddenInGame(true);
	NewISM->SetCastShadow(false);
	NewISM->RegisterComponent();

	GDebugISMs.Add(Placeable, NewISM);
	return NewISM;
}

void FPlaceableEditorDetails::RefreshShapeDebugVisualization(APlaceable* Placeable)
{
	UInstancedStaticMeshComponent* ISM = GetOrCreateDebugISM(Placeable);
	if (!ISM)
	{
		return;
	}

	ISM->ClearInstances();

	const UCargoEditorSettings* EditorSettings = GetDefault<UCargoEditorSettings>();
	UStaticMesh* CellMesh = EditorSettings->DebugCellMesh.LoadSynchronous();
	UMaterialInterface* CellMaterial = EditorSettings->DebugCellMaterial.LoadSynchronous();

	if (!ISM->GetStaticMesh() && CellMesh)
	{
		ISM->SetStaticMesh(CellMesh);
	}

	if (CellMaterial)
	{
		ISM->SetMaterial(0, CellMaterial);
	}

	const float CellSize = GetDefault<UCargoSettings>()->GridCellSize;
	const float BoxScale = (CellSize * 0.9f) / 100.f;

	const TArray<FIntVector> RotatedCells = Placeable->GridShapeDefinition.GetRotatedCells(Placeable->LocalYaw);

	for (const FIntVector& Cell : RotatedCells)
	{
		const FVector CellCenterLocal(
			(Cell.X - 0.5f) * CellSize,
			(Cell.Y - 0.5f) * CellSize,
			(Cell.Z - 0.5f) * CellSize);

		FTransform InstanceTransform;
		InstanceTransform.SetLocation(CellCenterLocal);
		InstanceTransform.SetScale3D(FVector(BoxScale));

		ISM->AddInstance(InstanceTransform);
	}
}

#undef LOCTEXT_NAMESPACE