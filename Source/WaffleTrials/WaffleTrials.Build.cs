// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WaffleTrials : ModuleRules
{
	public WaffleTrials(ReadOnlyTargetRules Target) : base(Target)
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

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Paper2D", "UMG", "Slate", "SlateCore" });
		
		PublicIncludePaths.AddRange(new string[] {
			"WaffleTrials",
			"WaffleTrials/Variant_Platforming",
			"WaffleTrials/Variant_Platforming/Animation",
			"WaffleTrials/Variant_Combat",
			"WaffleTrials/Variant_Combat/AI",
			"WaffleTrials/Variant_Combat/Animation",
			"WaffleTrials/Variant_Combat/Gameplay",
			"WaffleTrials/Variant_Combat/Interfaces",
			"WaffleTrials/Variant_Combat/UI",
			"WaffleTrials/Variant_SideScrolling",
			"WaffleTrials/Variant_SideScrolling/AI",
			"WaffleTrials/Variant_SideScrolling/Gameplay",
			"WaffleTrials/Variant_SideScrolling/Interfaces",
			"WaffleTrials/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
