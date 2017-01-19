package improbable.util

import improbable.fapi.engine._
import improbable.papi.engine.EngineConstraint

class UnrealFsimConstraintToEngineDescriptorResolver extends ConstraintToEngineDescriptorResolver {
  override def getEngineDescriptorForConstraint(engineConstraint: EngineConstraint): Option[EngineDescriptor] = {
    engineConstraint match {
      case UnrealFsimConstraint =>
        Some(UnrealFsimDescriptor)
      case _ =>
        None
    }
  }
}

object UnrealFsimConstraintToEngineDescriptorResolver extends UnrealFsimConstraintToEngineDescriptorResolver

object UnrealFsimConstraintResolver extends CompositeEngineDescriptorResolver(
  UnrealFsimConstraintToEngineDescriptorResolver
)