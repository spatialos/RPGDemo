// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;
using System.IO;

public class unreal : SpatialOSModuleRules
{
	/// <summary>
    /// Schema files are processed and output to this folder. It should be within
    /// the current module so the types are accessible to the game.
    /// </summary>
    protected string GeneratedCodeDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Improbable", "Generated")); }
    }

    public unreal(TargetInfo Target) : base(Target)
	{
		if (UEBuildConfiguration.bCleanProject)
        {
            RunSpatial("process_schema clean --language=cpp_unreal " + QuoteString(GeneratedCodeDir));
        }
        else
        {
            RunSpatial("process_schema --use_worker_defaults --language=cpp_unreal --output=" + QuoteString(GeneratedCodeDir));       
        }
		
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "SpatialOSGenerated" });

	    PublicIncludePaths.AddRange(new[]
	    {
	        Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "SpatialOSGenerated", "CoreLibrary")),
	        Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "SpatialOSGenerated", "Usr")),
	    });
		
		PrivateIncludePaths.AddRange(new[]
		{
			Path.GetFullPath(Path.Combine(GeneratedCodeDir))
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
