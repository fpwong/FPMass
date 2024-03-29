﻿// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPMass : ModuleRules
{
	public FPMass(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
				"FPVertexAnimationTextures",
				"MassRepresentation",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...
				"StructUtils",

				// ~~ Mass
				"MassEntity",
				"MassCommon",
				"MassMovement",
				"MassActors",
				"MassSpawner",
				"MassGameplayDebug",
				"MassSignals",
				"MassCrowd",
				"MassActors",
				"MassSpawner",
				"MassReplication",
				"MassNavigation",
				"MassLOD",
				"MassSimulation",

				// ~~ Other
				"GameplayAbilities",
				"AnimToTexture",
				"NavigationSystem",
				"GameplayTags",
				"DeveloperSettings",

				// ~~ Custom plugins
				"FPGameplayAbilities",
				"MassCommunitySample"
			}
		);
	}
}
