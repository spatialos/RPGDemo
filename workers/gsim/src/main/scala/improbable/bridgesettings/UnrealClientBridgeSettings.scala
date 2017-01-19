package improbable.bridgesettings

import improbable.fapi.bridge.{BridgeSettings, BridgeSettingsResolver, ConstantEngineLoadPolicy, PerEntityOrderedStateUpdateQos}
import improbable.fapi.network.RakNetLinkSettings
import improbable.unity.fabric._
import improbable.unity.fabric.bridge.ClientAssetContextDiscriminator
import improbable.unity.fabric.satisfiers.SatisfyVisual

object UnrealClientBridgeSettings extends BridgeSettingsResolver {

  val UNREAL_CLIENT_TYPE = "UnrealClient"

  private val UNREAL_CLIENT_ENGINE_BRIDGE_SETTINGS = BridgeSettings(
    ClientAssetContextDiscriminator(),
    RakNetLinkSettings(),
    UNREAL_CLIENT_TYPE,
    SatisfyVisual,
    AuthoritativeEntityOnly(),
    ConstantEngineLoadPolicy(0.5),
    PerEntityOrderedStateUpdateQos
  )

  private val bridgeSettings = Map[String, BridgeSettings](
    UNREAL_CLIENT_TYPE -> UNREAL_CLIENT_ENGINE_BRIDGE_SETTINGS
  )

  override def engineTypeToBridgeSettings(engineType: String, metadata: String): Option[BridgeSettings] = {
    bridgeSettings.get(engineType)
  }
}
