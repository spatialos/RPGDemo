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
            RunSpatial("process_schema clean --language=cpp_unreal " + QuoteString(GeneratedCodeDir));
        }
        else
        {
            RunSpatial("process_schema --use_worker_defaults --language=cpp_unreal --output=" + QuoteString(GeneratedCodeDir));
            PublicDependencyModuleNames.Add("SpatialOS");
        }

        PrivateIncludePaths.AddRange(new string[]
        {
            Path.Combine(GeneratedCodeDir),
            Path.Combine(WorkerSdkDir, "include"),
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
            File.WriteAllText(Path.Combine(TempDir, string.Format("UnrealFsim.bat", ModuleName)), Content);
            RunSpatial(string.Format("file zip --basePath=\"{0}\" --output=../../build/assembly/worker/UnrealFsim@Windows *.bat", TempDir, ModuleName));
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