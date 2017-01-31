// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;
using System.IO;

public class SpatialOSGenerated : SpatialOSModuleRules
{
    private string CoreLibraryDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "CoreLibrary")); }
    }
	
	private string StandardLibraryDir
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Std")); }
	}

    public SpatialOSGenerated(TargetInfo Target) : base(Target)
    {
        if (UEBuildConfiguration.bCleanProject)
        {

        }
        else
        {
			var std = "process_schema --cachePath=.spatialos/schema_codegen_cache_std" +
                " --output=" + QuoteString(StandardLibraryDir) +
                " --language=cpp_unreal" +
                " --intermediate_proto_dir=.spatialos/schema_codegen_proto_std" +
                " --input=../../build/dependencies/schema/WorkerSdkSchema";

			RunSpatial(std);

            var cl = "process_schema --cachePath=.spatialos/schema_codegen_cache_cl" +
                " --output=" + QuoteString(CoreLibraryDir) +
                " --language=cpp_unreal" +
                " --intermediate_proto_dir=.spatialos/schema_codegen_proto_cl" +
                " --input=../../build/dependencies/schema/CoreLibrary";

            RunSpatial(cl);

        }
        PublicIncludePaths.AddRange(new string[]
        {
			Path.GetFullPath(StandardLibraryDir),
            Path.GetFullPath(CoreLibraryDir)
        });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "Engine" });
        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
