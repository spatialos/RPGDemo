package improbable.natures

import improbable.behaviours.AddEngineConstraintsForPlayer
import improbable.behaviours.delegates.{DelegateCheckIsPlayer, DelegateTransformStateToClient}
import improbable.check.CheckIsPlayer
import improbable.corelib.natures.{BaseNature, NatureApplication, NatureDescription}
import improbable.corelib.util.EntityOwner
import improbable.corelibrary.transforms.TransformNature
import improbable.math.Vector3d
import improbable.papi.engine.EngineId
import improbable.papi.entity.behaviour.EntityBehaviourDescriptor
import improbable.util.EntityPrefabs._

object PlayerNature extends NatureDescription {

  override def dependencies: Set[NatureDescription] = Set(
    BaseNature,
    TransformNature
  )

  override def activeBehaviours: Set[EntityBehaviourDescriptor] = {
    Set(
      descriptorOf[DelegateCheckIsPlayer],
      descriptorOf[DelegateTransformStateToClient],
      descriptorOf[AddEngineConstraintsForPlayer]
    )
  }

  def apply(engineId: EngineId): NatureApplication = {
    application(
      states = Seq(
        EntityOwner(ownerId = Some(engineId)),
        CheckIsPlayer()
      ),
      natures = Seq(
        BaseNature(entityPrefab = PLAYER, isPhysical = false),
        TransformNature(globalPosition = Vector3d(0, 10, 0))
      )
    )
  }

}
