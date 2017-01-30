// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SpatialOSGenerated : SpatialOSModuleRules
{
    private string CoreLibraryDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "CoreLibrary")); }
    }

    /// <summary>
    /// Schema files are processed and output to this folder. It should be within
    /// the current module so the types are accessible to the game.
    /// </summary>
    protected string UserGeneratedCodeDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Usr")); }
    }

    public SpatialOSGenerated(TargetInfo Target) : base(Target)
    {
        if (UEBuildConfiguration.bCleanProject)
        {

        }
        else
        {
            var usr = "process_schema --cachePath=.spatialos/schema_codegen_cache_usr" +
                " --output=" + QuoteString(UserGeneratedCodeDir) +
                " --language=cpp_unreal" +
                " --intermediate_proto_dir=.spatialos/schema_codegen_proto_usr" +
                " --input=../../schema" +
                " --repository=../../build/dependencies/schema";

            RunSpatial(usr);

            var cl = "process_schema --cachePath=.spatialos/schema_codegen_cache_cl" +
                " --output=" + QuoteString(CoreLibraryDir) +
                " --language=cpp_unreal" +
                " --intermediate_proto_dir=.spatialos/schema_codegen_proto_cl" +
                " --input=../../build/dependencies/schema/CoreLibrary";

            RunSpatial(cl);

        }
        PublicIncludePaths.AddRange(new string[]
        {
            Path.GetFullPath(UserGeneratedCodeDir),
            Path.GetFullPath(CoreLibraryDir)
        });

        PublicDependencyModuleNames.AddRange(new string[] { "Engine" });
        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
