// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SpatialOSGenerated : SpatialOSModuleRules
{
    private string CoreLibraryDir
    {
        get { return Path.GetFullPath(Path.Combine(GeneratedCodeDir, "CoreLibrary")); }
    }

    public SpatialOSGenerated(TargetInfo Target) : base(Target)
    {
        // Add a dependency on CoreLibrary.
        WorkerPackage("unreal", "improbable_corelibrary", CoreLibraryDir);
        PrivateIncludePaths.Add(Path.Combine(CoreLibraryDir, "Generated"));

        PublicIncludePaths.AddRange(new string[]
        {
            Path.GetFullPath(Path.Combine(ModuleDirectory, "CoreLibrary", "Generated")),
            Path.GetFullPath(Path.Combine(ModuleDirectory, "Src"))
        });

        PublicDependencyModuleNames.AddRange(new string[] { "Core" });
        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
