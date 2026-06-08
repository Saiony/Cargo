// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Cargo : ModuleRules
{
	public Cargo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Cargo",
			"Cargo/Variant_Platforming",
			"Cargo/Variant_Platforming/Animation",
			"Cargo/Variant_Combat",
			"Cargo/Variant_Combat/AI",
			"Cargo/Variant_Combat/Animation",
			"Cargo/Variant_Combat/Gameplay",
			"Cargo/Variant_Combat/Interfaces",
			"Cargo/Variant_Combat/UI",
			"Cargo/Variant_SideScrolling",
			"Cargo/Variant_SideScrolling/AI",
			"Cargo/Variant_SideScrolling/Gameplay",
			"Cargo/Variant_SideScrolling/Interfaces",
			"Cargo/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
