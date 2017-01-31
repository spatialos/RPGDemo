// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;
using System.IO;

public class unreal : SpatialOSModuleRules
{

    public unreal(TargetInfo Target) : base(Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "SpatialOSGenerated" });

	    PublicIncludePaths.AddRange(new[]
	    {
	        Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "SpatialOSGenerated", "CoreLibrary")),
	        Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "SpatialOSGenerated", "Usr")),
	    });

	    System.Console.WriteLine(Path.Combine(ModuleDirectory, "SpatialOSGenerated", "CoreLibrary"));

        switch (Target.Configuration)
        {
            case UnrealTargetConfiguration.Shipping:
                Definitions.Add("UNREAL_FSIM=1");
                break;

            default:
                Definitions.Add("UNREAL_CLIENT=1");
                break;
        }
    }
}
