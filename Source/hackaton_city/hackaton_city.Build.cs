// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class hackaton_city : ModuleRules
{
	public hackaton_city(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "WaveFunctionCollapse", "UnrealEd", "DeveloperSettings" });
	}
}
