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
	
	private string CoreLibraryDir
    {
        get { return Path.GetFullPath(Path.Combine(GeneratedCodeDir, "CoreLibrary")); }
    }
	
	private string StandardLibraryDir
	{
		get { return Path.GetFullPath(Path.Combine(GeneratedCodeDir, "Std")); }
	}
	
	private string UserSchemaDir
	{
		get { return Path.GetFullPath(Path.Combine(GeneratedCodeDir, "Usr")); }
	}

    public unreal(TargetInfo Target) : base(Target)
	{
		if (UEBuildConfiguration.bCleanProject)
        {
            RunSpatial("process_schema clean --language=cpp_unreal " + QuoteString(GeneratedCodeDir));
        }
        else
        {				
            var cl = "process_schema --cachePath=.spatialos/schema_codegen_cache_cl" +
                " --output=" + QuoteString(CoreLibraryDir) +
                " --language=cpp_unreal" +
                " --intermediate_proto_dir=.spatialos/schema_codegen_proto_cl" +
                " --input=../../build/dependencies/schema/CoreLibrary";

            RunSpatial(cl);
			
			var std = "process_schema --cachePath=.spatialos/schema_codegen_cache_std" +
                " --output=" + QuoteString(StandardLibraryDir) +
                " --language=cpp_unreal" +
                " --intermediate_proto_dir=.spatialos/schema_codegen_proto_std" +
                " --input=../../build/dependencies/schema/WorkerSdkSchema";

			RunSpatial(std);
			
			var user = "process_schema --cachePath=.spatialos/schema_codegen_cache_usr" +
                " --output=" + QuoteString(UserSchemaDir) +
                " --language=cpp_unreal" +
                " --intermediate_proto_dir=.spatialos/schema_codegen_proto_usr" +
                " --input=../../schema";
				
            RunSpatial(user);       
        }
		
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

	    PublicIncludePaths.AddRange(new[]
	    {
	        Path.GetFullPath(CoreLibraryDir),
	        Path.GetFullPath(StandardLibraryDir),
			Path.GetFullPath(UserSchemaDir)
	    });

        switch (Target.Type)
        {
            case TargetRules.TargetType.Server:
                Definitions.Add("UNREAL_FSIM=1");
                break;

            default:
                Definitions.Add("UNREAL_CLIENT=1");
                break;
        }
    }
}
