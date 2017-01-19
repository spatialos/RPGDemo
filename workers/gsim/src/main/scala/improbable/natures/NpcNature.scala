package improbable.natures

import improbable.behaviours.delegates.DelegateTransformStateToFsim
import improbable.corelib.natures.{BaseNature, NatureApplication, NatureDescription}
import improbable.corelibrary.transforms.TransformNature
import improbable.math.Vector3d
import improbable.papi.entity.behaviour.EntityBehaviourDescriptor
import improbable.util.EntityPrefabs._

object NpcNature extends NatureDescription {

  override def dependencies: Set[NatureDescription] = Set(
    BaseNature,
    TransformNature
  )

  override def activeBehaviours: Set[EntityBehaviourDescriptor] = {
    Set(
      descriptorOf[DelegateTransformStateToFsim]
    )
  }

  def apply(position: Vector3d): NatureApplication = {
    application(
      states = Seq(),
      natures = Seq(
        BaseNature(entityPrefab = NPC, isPhysical = false),
        TransformNature(globalPosition = position)
      )
    )
  }

}
