// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class unreal : SpatialOSModuleRules
{
    private string CoreLibraryDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Improbable", "CoreLibrary")); }
    }

    public unreal(TargetInfo Target) : base(Target)
	{
        // Add a dependency on CoreLibrary.
        WorkerPackage("unreal", "improbable_corelibrary", CoreLibraryDir);
        PrivateIncludePaths.Add(Path.Combine(CoreLibraryDir, "Generated"));

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

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
