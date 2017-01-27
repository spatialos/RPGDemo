// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using UnrealBuildTool;

/// <summary>
/// This class provides a default configuration for using the SpatialOS WorkerSdk.
/// Inherit your game's ModuleRules from this class to automatically download the appropriate
/// Worker Packages and setup include and library paths.
/// </summary>
public class SpatialOSModuleRules : ModuleRules
{
    /// <summary>
    /// Schema files are processed and output to this folder. It should be within
    /// the current module so the types are accessible to the game.
    /// </summary>
    protected string GeneratedCodeDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Improbable", "Generated")); }
    }
	
	/// <summary>
	/// Schema files for the standard SpatialOS library are processed and output to this folder.
	/// It should be within the current module so the types are accessible to the game.
	protected string GeneratedStdCodeDir
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Improbable", "Std")); }
	}
	
	/// <summary>
	/// Path to the generated codegen cache for the Spatial standard library.
	protected string GeneratedStdCodegenCacheDir
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", ".spatialos", "schema_codegen_cache_std")); }
	}
	
	/// <summary>
	/// Path to the generated codegen proto for user schemas.
	protected string GeneratedStdCodegenProtoDir
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", ".spatialos", "schema_codegen_proto_std")); }
	}
	
	/// <summary>
	/// User schema files are processed and output to this folder.
	/// It should be within the current module so the types are accessible to the game.
	protected string GeneratedUserCodeDir
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Improbable", "Usr")); }
	}
	
	/// <summary>
	/// Path to the generated codegen cache for user schemas.
	protected string GeneratedUserCodegenCacheDir
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", ".spatialos", "schema_codegen_cache_usr")); }
	}
	
	/// <summary>
	/// Path to the generated codegen proto for the Spatial standard library.
	protected string GeneratedUserCodegenProtoDir
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", ".spatialos", "schema_codegen_proto_usr")); }
	}
	
	/// <summary>
	/// Path to the location of the WorkerSdkSchema schemas
	protected string WorkerSdkSchemasPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "..", "..", "build", "dependencies", "schema", "WorkerSdkSchema")); }
	}
	
	/// <summary>
	/// Path to the location of the user schemas
	protected string UserSchemasPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "..", "..", "schema")); }
	}
	
	/// <summary>
	/// Path to the location of the user schemas repository
	protected string UserSchemasRepositoryPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "..", "..", "build", "dependencies", "schema")); }
	}

    /// <summary>
    /// The default location of the SpatialOS Unreal plugin.
    /// </summary>
    protected string SpatialOSPluginDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "Plugins", "SpatialOS")); }
    }

    /// <summary>
    /// The default location of the WorkerSdk
    /// </summary>
    protected string WorkerSdkDir
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty", "Improbable", "WorkerSdk")); }
    }

    /// <summary>
    /// Surround a string quotes. Utility function for ensuring that paths are properly quoted for use on the command line.
    /// </summary>
    protected string QuoteString(string input)
    {
        return string.Format("\"{0}\"", input);
    }

    /// <summary>
    /// Manage a SpatialOS worker package.
    /// If UEBuildConfiguration.bCleanProject is true, TargetDir is removed.
    /// Otherwise, a package of the version matching the application's current Sdk version is downloaded.
    /// 
    /// See `spatialos.json`, "sdk_version".
    /// </summary>
    /// <param name="Type">The type of the package.</param>
    /// <param name="Name">The name of the package.</param>
    /// <param name="TargetDir">The directory to install the package to.</param>
    protected void WorkerPackage(string Type, string Name, string TargetDir)
    {
        if (UEBuildConfiguration.bCleanProject)
        {
            if (Directory.Exists(TargetDir))
            {
                Directory.Delete(TargetDir, true);
            }
        }
        else
        {
            RunSpatial(string.Format("worker_package unpack-to {0} {1} {2}", Type, Name, QuoteString(TargetDir)));
            //BuildWorkerPackageInAssembly();
        }
    }

    private SpatialOSModuleRules() { }

    protected SpatialOSModuleRules(TargetInfo Target)
    {
        var basePath = Path.GetFullPath(ModuleDirectory);
        var moduleDir = Path.GetFileName(basePath);

        WorkerPackage("worker_sdk", "windows_x64", WorkerSdkDir);

        if (UEBuildConfiguration.bCleanProject)
        {
			var cleanCommand = String.Format("process_schema clean --language=cpp_unreal {0} {1} {2} {3} {4} {5} {6}",
				QuoteString(GeneratedCodeDir),
				QuoteString(GeneratedStdCodeDir),
				QuoteString(GeneratedUserCodeDir),
				QuoteString(GeneratedStdCodegenCacheDir),
				QuoteString(GeneratedStdCodegenProtoDir),
				QuoteString(GeneratedUserCodegenCacheDir),
				QuoteString(GeneratedUserCodegenProtoDir));
				
			Console.WriteLine("About to call: " + cleanCommand);
            RunSpatial(cleanCommand);
        }
        else
        {
            RunSpatial("process_schema --use_worker_defaults --language=cpp_unreal --output=" + QuoteString(GeneratedCodeDir));
			
			var standardLibraryGenerationCommand = String.Format("process_schema --cachePath={0} --output={1} --intermediate_proto_dir={2} --input={3} --language=cpp_unreal", 
				GeneratedStdCodegenCacheDir,
				GeneratedStdCodeDir,
				GeneratedStdCodegenProtoDir,
				WorkerSdkSchemasPath);
				
			RunSpatial(standardLibraryGenerationCommand);
			
			var userSchemaGenerationCommand = String.Format("process_schema --cachePath={0} --output={1} --intermediate_proto_dir={2} --input={3} --repository={4} --language=cpp_unreal",
				GeneratedUserCodegenCacheDir,
				GeneratedUserCodeDir,
				GeneratedUserCodegenProtoDir,
				UserSchemasPath,
				UserSchemasRepositoryPath
			);

			RunSpatial(userSchemaGenerationCommand);
			
            PublicDependencyModuleNames.Add("SpatialOS");
        }

        PrivateIncludePaths.AddRange(new string[]
        {
            Path.Combine(GeneratedCodeDir),
            Path.Combine(WorkerSdkDir, "include"),
			GeneratedStdCodeDir,
			GeneratedUserCodeDir
        });

    }

    protected void RunSpatial(string command)
    {
        var p = new Process();
        p.StartInfo = new ProcessStartInfo
        {
            Arguments = command + " --log_level=warning",
            WorkingDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..")),
            FileName = "spatial",
            RedirectStandardError = true,
            RedirectStandardOutput = true,
            UseShellExecute = false,
            CreateNoWindow = true,
        };

        try
        {
            p.OutputDataReceived += (sender, eventArgs) => Console.WriteLine(eventArgs.Data);
            p.ErrorDataReceived += (sender, eventArgs) => Console.WriteLine(eventArgs.Data);

            Console.WriteLine("spatial " + command);

            p.Start();
            p.BeginErrorReadLine();
            p.BeginOutputReadLine();
            p.WaitForExit();
            if (p.ExitCode != 0)
            {
                throw new Exception(string.Format("Failed with exit code: {0} {1} {2}", p.ExitCode, p.StartInfo.FileName, p.StartInfo.Arguments));
            }
        }
        finally
        {
            p.Dispose();
        }
    }

    // UE4 command line flags:
    //
    // -server = Run the editor in server mode.
    // -stdout = Write logs to the console.
    // -nowrite = Don't write a logfile.
    // -unattended = Don't prompt for user input.
    // -nologtimes = Don't add timestamps to log messages.
    // -nopause = Close the window when the process finishes.
    // -noini = Don't try to write to INI (settings) files.
    // first "--" = Disable further command line parsing in the spatial tool (this won't be passed to Unreal's command line.)
    // second "--" = Marker for the WitnessGameMode that it should start parsing command line options.
    private const string BatchScript = @"cd ""{0}""
spatial invoke unreal editor -- -server -stdout -nowrite -unattended -nologtimes -nopause -noini -- %*";

    /// <summary>
    /// Writes a batch file and zips it up so that it can be executed on Windows via `spatial local`.
    /// </summary>
    /// <remarks>
    /// This will only work on the local machine. This is not suitable for cloud deployment.
    /// </remarks>
    private void BuildWorkerPackageInAssembly()
    {
        var ModuleName = Path.GetFileName(ModuleDirectory);
        var TempDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "SpatialOSBuildAssemblyTemp"));
        try
        {
            Directory.CreateDirectory(TempDir);

            var Content = string.Format(BatchScript, Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..")));
            File.WriteAllText(Path.Combine(TempDir, string.Format("UnrealFSim.bat", ModuleName)), Content);
            RunSpatial(string.Format("file zip --basePath=\"{0}\" --output=../../build/assembly/worker/UnrealFSim@Windows *.bat", TempDir, ModuleName));
        }
        finally
        {
            try
            {
                if (Directory.Exists(TempDir))
                {
                    Directory.Delete(TempDir, true);
                }
            }
            catch (System.Exception)
            {
                // Nothing sensible to do here.
            }
        }
    }
}