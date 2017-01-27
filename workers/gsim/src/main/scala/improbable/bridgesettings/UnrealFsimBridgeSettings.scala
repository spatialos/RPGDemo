package improbable.bridgesettings

import improbable.fapi.bridge._
import improbable.fapi.network.RakNetLinkSettings
import improbable.unity.fabric.AuthoritativeEntityOnly
import improbable.unity.fabric.bridge.FSimAssetContextDiscriminator
import improbable.util.UnrealFsimConstraintSatisfier

object UnrealFsimBridgeSettings extends BridgeSettingsResolver {

  val UNREAL_FSIM_TYPE = "UnrealFSim"

  private val UNREAL_FSIM_BRIDGE_SETTINGS = BridgeSettings(
    FSimAssetContextDiscriminator(),
    RakNetLinkSettings(),
    UNREAL_FSIM_TYPE,
    UnrealFsimConstraintSatisfier,
    AuthoritativeEntityOnly(),
    ConstantEngineLoadPolicy(0.5),
    PerEntityOrderedStateUpdateQos
  )

  private val bridgeSettings = Map[String, BridgeSettings](
    UNREAL_FSIM_TYPE -> UNREAL_FSIM_BRIDGE_SETTINGS
  )

  override def engineTypeToBridgeSettings(engineType: String, metadata: String): Option[BridgeSettings] = {
    bridgeSettings.get(engineType)
  }
}

