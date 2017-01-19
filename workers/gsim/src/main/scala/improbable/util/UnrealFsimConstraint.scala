package improbable.util

import improbable.fapi.engine.ProtoEngineConstraintSatisfier
import improbable.papi.engine.{EngineConstraints, ProtoEngineConstraint}

case object UnrealFsimConstraintSatisfier extends ProtoEngineConstraintSatisfier(
  EngineConstraints.makeClaim(EngineConstraints.makeAtom("unrealfsim"))
)

case object UnrealFsimConstraint extends ProtoEngineConstraint(EngineConstraints.makePredicate("unrealfsim"))