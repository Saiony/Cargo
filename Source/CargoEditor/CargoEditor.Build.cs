using UnrealBuildTool;

public class CargoEditor : ModuleRules
{
    public CargoEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "Cargo",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "PropertyEditor",
                "Cargo",
                "AssetTools",
                "UnrealEd",
                "InputCore",
                "DeveloperSettings"
            }
        );
    }
}