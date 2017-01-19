package improbable.behaviours.delegates

import improbable.corelib.util.EntityOwnerUtils
import improbable.corelibrary.transforms.TransformState
import improbable.papi.entity.{Entity, EntityBehaviour}
import improbable.unity.papi.SpecificEngineConstraint

class DelegateTransformStateToClient (entity: Entity) extends EntityBehaviour{
  override def onReady(): Unit = {
    entity.delegateState[TransformState](SpecificEngineConstraint(EntityOwnerUtils.ownerIdOf(entity)))
  }
}
