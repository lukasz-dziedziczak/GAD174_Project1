// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAD174_Project1 : ModuleRules
{
	public GAD174_Project1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Paper2D" });
	}
}
