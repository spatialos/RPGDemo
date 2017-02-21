
#pragma once

#include "Core.h"

#include "improbable/math/coordinates.h"
#include <improbable/collections.h>

extern const FRotator SpatialToUnrealCoordinateSpace;
constexpr double SpatialToUnrealScale = 100.0;

inline improbable::math::Coordinates ToNativeCoordinates(const FVector& position)
{
    const auto spatialPosition = SpatialToUnrealCoordinateSpace.GetInverse().RotateVector(position) / SpatialToUnrealScale;
    return improbable::math::Coordinates{ static_cast<double>(spatialPosition.X),
                                                static_cast<double>(spatialPosition.Y),
                                                static_cast<double>(spatialPosition.Z) };
}

inline FVector ToUnrealPosition(const improbable::math::Coordinates& position)
{
  auto result = FVector{static_cast<float>(position.x()),
                        static_cast<float>(position.y()),
                        static_cast<float>(position.z())};
  result = SpatialToUnrealCoordinateSpace.RotateVector(result);
  result *= SpatialToUnrealScale;

  return result;
}

inline worker::List<float> ToNativeRotation(const FQuat quaternion)                  
{                                                                                                  
    const FQuat rotatedQuaternion = SpatialToUnrealCoordinateSpace.GetInverse().Quaternion() * quaternion;
    return worker::List<float>{rotatedQuaternion.W, rotatedQuaternion.X, rotatedQuaternion.Y, rotatedQuaternion.Z};
}                                                                                                  
                                                                                                   
inline FQuat ToUnrealRotation(const worker::List<float>& quaterion)                                         
{                                                                                                  
    const FQuat result{ quaterion[1], quaterion[2], quaterion[3], quaterion[0] };
    return SpatialToUnrealCoordinateSpace.Quaternion() * result;
}                                                                                                  