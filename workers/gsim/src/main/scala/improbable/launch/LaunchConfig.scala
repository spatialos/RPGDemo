package improbable.launch

import improbable.apps._
import improbable.dapi.LaunchConfig
import improbable.util.UnrealFsimConstraintResolver

class DemonstrationLaunchConfig(useDynamicEngineSpooling: Boolean) extends {} with LaunchConfig(
  AppList.ALL,
  useDynamicEngineSpooling,
  BridgeSettingsResolver,
  UnrealFsimConstraintResolver
)

object ManualEngineStartupLaunchConfig extends DemonstrationLaunchConfig(useDynamicEngineSpooling = false)

object AutomaticEngineStartupLaunchConfig extends DemonstrationLaunchConfig(useDynamicEngineSpooling = false)
