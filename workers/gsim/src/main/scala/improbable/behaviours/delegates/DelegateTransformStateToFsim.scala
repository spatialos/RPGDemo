package improbable.behaviours.delegates

import improbable.corelibrary.transforms.TransformState
import improbable.papi.entity.{Entity, EntityBehaviour}
import improbable.util.UnrealFsimConstraint

class DelegateTransformStateToFsim (entity: Entity) extends EntityBehaviour{
  override def onReady(): Unit = {
    entity.addEngineConstraint(UnrealFsimConstraint)
    entity.delegateState[TransformState](UnrealFsimConstraint)
  }
}
