// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FrogsmithUI : ModuleRules
{
	public FrogsmithUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			[
				"Core",
				"CoreUObject",
				"InputCore",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"CommonInput",
				"CommonUI",
				"GameplayTags", 
				"DeveloperSettings"
			]
		);


		PrivateDependencyModuleNames.AddRange(
			[
			]
		);


		DynamicallyLoadedModuleNames.AddRange(
			[
			]
		);
	}
}