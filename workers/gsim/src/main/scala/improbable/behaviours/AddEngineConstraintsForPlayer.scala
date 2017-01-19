package improbable.behaviours

import improbable.corelib.util.EntityOwnerUtils
import improbable.papi.entity.{Entity, EntityBehaviour}
import improbable.unity.papi.SpecificEngineConstraint
import improbable.util.UnrealFsimConstraint

class AddEngineConstraintsForPlayer(entity: Entity) extends EntityBehaviour {

  override def onReady(): Unit = {
    entity.addEngineConstraint(UnrealFsimConstraint)
    entity.addEngineConstraint(SpecificEngineConstraint(EntityOwnerUtils.ownerIdOf(entity)))
  }

}
