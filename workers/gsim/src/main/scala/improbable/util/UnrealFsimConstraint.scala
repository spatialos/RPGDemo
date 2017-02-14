package improbable.util

import improbable.fapi.engine.ProtoEngineConstraintSatisfier
import improbable.papi.engine.{EngineConstraints, ProtoEngineConstraint}

case object UnrealFsimConstraintSatisfier extends ProtoEngineConstraintSatisfier(
  EngineConstraints.makeAttributeSet(EngineConstraints.makeAttribute("UnrealWorker"))
)

case object UnrealFsimConstraint extends ProtoEngineConstraint(EngineConstraints.makeRequirementSet("UnrealWorker"))