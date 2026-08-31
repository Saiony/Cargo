#include "CargoEditor.h"

#include "GridComponentDADetails.h"
#include "PlaceableEditor.h"
#include "DataAssets/GridComponentDA.h"


void FCargoEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	
	PropertyModule.RegisterCustomClassLayout("Placeable", FOnGetDetailCustomizationInstance::CreateStatic(&FPlaceableEditorDetails::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("GridComponentDA", FOnGetDetailCustomizationInstance::CreateStatic(&FGridComponentDADetails::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();}

void FCargoEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		return;
	
	
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout("Placeable");
	PropertyModule.UnregisterCustomClassLayout(UGridComponentDA::StaticClass()->GetFName());
}
    
IMPLEMENT_MODULE(FCargoEditorModule, CargoEditor)
