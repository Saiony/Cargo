#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class APlaceable;
class UInstancedStaticMeshComponent;
class IDetailLayoutBuilder;
class IDetailCategoryBuilder;

class FPlaceableEditorDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	FReply OnRefreshDataClicked(APlaceable* Placeable);

	void LoadShape(APlaceable* Placeable);
	void RefreshShapeDebugVisualization(APlaceable* Placeable);

	UInstancedStaticMeshComponent* GetOrCreateDebugISM(APlaceable* Placeable);

	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
};