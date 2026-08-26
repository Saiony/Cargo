#include "CargoEditor.h"

#include "PlaceableEditor.h"


void FCargoEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	
	PropertyModule.RegisterCustomClassLayout("Placeable", FOnGetDetailCustomizationInstance::CreateStatic(&FPlaceableEditorDetails::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();}

void FCargoEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		return;
	
	
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout("Placeable");
}
    
IMPLEMENT_MODULE(FCargoEditorModule, CargoEditor)
