package improbable.launch

import improbable.bridgesettings.{UnrealClientBridgeSettings, UnrealFsimBridgeSettings}
import improbable.fapi.bridge.CompositeBridgeSettingsResolver

object BridgeSettingsResolver extends CompositeBridgeSettingsResolver(
  UnrealClientBridgeSettings,
  UnrealFsimBridgeSettings
)
