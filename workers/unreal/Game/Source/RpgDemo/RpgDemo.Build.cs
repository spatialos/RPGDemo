// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

using System;
using System.Collections.Generic;
using UnrealBuildTool;
using System.IO;

public class RpgDemo : ModuleRules
{
	public RpgDemo(TargetInfo Target)
	{
        var SpatialOS = new SpatialOSModule(this, Target, this.GetType().Name);
		SpatialOS.SetupSpatialOS();
		SpatialOS.GenerateCode();

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
	}
}
