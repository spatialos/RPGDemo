package improbable.behaviours.delegates

import improbable.check.CheckIsPlayer
import improbable.corelib.util.EntityOwnerDelegation.entityOwnerDelegation
import improbable.corelib.util.EntityOwnerUtils
import improbable.papi.entity.{Entity, EntityBehaviour}
import improbable.unity.papi.SpecificEngineConstraint

class DelegateCheckIsPlayer(entity: Entity) extends EntityBehaviour {

  override def onReady(migrating: Boolean): Unit = {
    entity.delegateState[CheckIsPlayer](SpecificEngineConstraint(EntityOwnerUtils.ownerIdOf(entity)))
  }

}
