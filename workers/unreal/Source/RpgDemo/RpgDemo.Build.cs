// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

using System;
using System.Collections.Generic;
using UnrealBuildTool;
using System.IO;

public class RpgDemo : ModuleRules
{
	/// <summary>
    /// Schema files are processed and output to this folder. It should be within
    /// the current module so the types are accessible to the game.
    /// </summary>
    protected string GeneratedCodeDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Improbable", "Generated", "cpp")); }
    }

	private string StandardLibraryDir
	{
		get { return Path.GetFullPath(Path.Combine(GeneratedCodeDir, "Std")); }
	}

	private string UserSchemaDir
	{
		get { return Path.GetFullPath(Path.Combine(GeneratedCodeDir, "Usr")); }
	}

	public RpgDemo(TargetInfo Target)
	{
        var SpatialOS = new SpatialOSModule(this, Target);
		SpatialOS.SetupSpatialOS();
		if (UEBuildConfiguration.bCleanProject)
		{
			SpatialOS.RunSpatial("process_schema clean " + SpatialOS.QuoteString(GeneratedCodeDir));
		}
		else
		{
			var addProjectIncludes = "invoke" +
				" unreal" +
				" add_project_includes" +
				" RpgDemo" +
				" Source/RpgDemo/SpatialOS/uclasses/**/*.h" +
				" Source/RpgDemo/SpatialOS/uclasses/**/*.cpp";

			SpatialOS.RunSpatial(addProjectIncludes);

			var stdCpp = "process_schema generate --cachePath=.spatialos/schema_codegen_cache_std" +
				" --output=" + SpatialOS.QuoteString(StandardLibraryDir) +
				" --language=cpp_unreal" +
				" --input=../../build/dependencies/schema/WorkerSdkSchema";

			SpatialOS.RunSpatial(stdCpp);

			var userCpp = "process_schema generate --cachePath=.spatialos/schema_codegen_cache_usr" +
				" --output=" + SpatialOS.QuoteString(UserSchemaDir) +
				" --language=cpp_unreal" +
				" --input=../../schema";

			SpatialOS.RunSpatial(userCpp);

			var stdJson = "process_schema" +
	            " generate" +
	            " --cachePath=.spatialos/schema_codegen_cache_json_std" +
	            " --output=Source/RpgDemo/Improbable/Generated/json/Std" +
	            " --language=ast_json" +
	            " --input=../../build/dependencies/schema/WorkerSdkSchema";

	        SpatialOS.RunSpatial(stdJson);

	        var userJson = "process_schema" +
				" generate" +
				" --cachePath=.spatialos/schema_codegen_cache_json_usr" +
				" --output=Source/RpgDemo/Improbable/Generated/json/Usr" +
				" --language=ast_json" +
				" --input=../../schema" +
				" --repository=../../build/dependencies/schema";

			SpatialOS.RunSpatial(userJson);		
			
			var CodeGenWorkingDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", ".spatialos", "bin"));
			var CodegenFilename = Path.GetFullPath(Path.Combine(CodeGenWorkingDirectory, "CodeGenerator.exe"));
			
			var CodegenArguments = String.Format("--json-dir={0} --unreal-output-dir={1} --unreal-project-name={2}", 
				SpatialOS.QuoteString(Path.GetFullPath(Path.Combine(ModuleDirectory, "Improbable", "Generated", "json"))),
				SpatialOS.QuoteString(Path.GetFullPath(Path.Combine(ModuleDirectory, "Improbable", "Generated", "cpp", "unreal"))),
				"RpgDemo");
			SpatialOS.RunExe(CodegenFilename, CodegenArguments, CodeGenWorkingDirectory);
		}

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PublicIncludePaths.AddRange(new[]
		{
			Path.GetFullPath(StandardLibraryDir),
				   Path.GetFullPath(UserSchemaDir)
		});
	}
}
