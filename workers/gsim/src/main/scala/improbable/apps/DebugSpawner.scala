package improbable.apps

import improbable.logging.Logger
import improbable.math.Vector3d
import improbable.natures.NpcNature
import improbable.papi.world.AppWorld
import improbable.papi.worldapp.WorldApp

class DebugSpawner(appWorld: AppWorld, logger: Logger) extends WorldApp {

  for (i <- 1 to 5) {
    appWorld.entities.spawnEntity(NpcNature(Vector3d(i, 10, 2)))
  }

}
