package improbable.apps

import improbable.bridgesettings.UnrealClientBridgeSettings
import improbable.logging.Logger
import improbable.natures.PlayerNature
import improbable.papi._
import improbable.papi.engine.EngineId
import improbable.papi.world.AppWorld
import improbable.papi.world.messaging.{EngineConnected, EngineDisconnected}
import improbable.papi.worldapp.WorldApp
import improbable.unity.fabric.engine.EnginePlatform

class PlayerLifeCycleManager(appWorld: AppWorld, logger: Logger) extends WorldApp {

  private var userIdToEntityIdMap = Map[EngineId, EntityId]()

  appWorld.messaging.subscribe {
    case engineConnectedMsg: EngineConnected =>
      engineConnected(engineConnectedMsg)

    case engineDisconnectedMsg: EngineDisconnected =>
      engineDisconnected(engineDisconnectedMsg)
  }

  private def engineConnected(msg: EngineConnected): Unit = {
    msg match {
      case EngineConnected(clientId, EnginePlatform.UNITY_CLIENT_ENGINE, _) =>
        addEntity(clientId)
      case EngineConnected(clientId, UnrealClientBridgeSettings.UNREAL_CLIENT_TYPE, _) =>
        addEntity(clientId)
      case _ =>
    }
  }

  private def engineDisconnected(msg: EngineDisconnected): Unit = {
    msg match {
      case EngineDisconnected(clientId, EnginePlatform.UNITY_CLIENT_ENGINE) =>
        removeUserIdToEntityIdEntry(clientId)
      case EngineDisconnected(clientId, UnrealClientBridgeSettings.UNREAL_CLIENT_TYPE) =>
        removeUserIdToEntityIdEntry(clientId)
      case _ =>
    }
  }

  private def addEntity(clientId: EngineId): Unit = {
    val playerEntityId = appWorld.entities.spawnEntity(PlayerNature(engineId = clientId))
    logger.info(s"Spawning Player with clientId $clientId and entityId $playerEntityId")
    userIdToEntityIdMap += clientId -> playerEntityId
  }

  private def removeUserIdToEntityIdEntry(clientId: EngineId) = {
    userIdToEntityIdMap.get(clientId) match {
      case Some(entityId) =>
        appWorld.entities.destroyEntity(entityId)
        logger.info(s"Destroying player: $clientId with entityId $entityId")
      case None =>
        logger.warn(s"User disconnected but could not find entity id for client: $clientId")
    }
  }
}
