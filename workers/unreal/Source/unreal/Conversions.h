
#pragma once

#include "Core.h"

#include "improbable/math/coordinates.h"
#include <improbable/collections.h>

extern const FRotator SpatialToUnrealCoordinateSpace;
constexpr double SpatialToUnrealScale = 100.0;

inline improbable::math::Coordinates ToNativeCoordinates(const FVector& position)
{
    auto spatialPosition = SpatialToUnrealCoordinateSpace.GetInverse().RotateVector(position) / SpatialToUnrealScale;
    return improbable::math::Coordinates{ static_cast<float>(spatialPosition.X),
                                                static_cast<float>(spatialPosition.Y),
                                                static_cast<float>(spatialPosition.Z) };
}

inline FVector ToUnrealPosition(const improbable::math::Coordinates& postion)
{
  auto result = FVector(static_cast<float>(postion.x),
                        static_cast<float>(postion.y),
                        static_cast<float>(postion.z));
  result = SpatialToUnrealCoordinateSpace.RotateVector(result);
  result *= SpatialToUnrealScale;

  return result;
}

inline worker::List<float> ToNativeRotation(const FQuat quaternion)                  
{                                                                                                  
    FQuat rotatedQuaternion = SpatialToUnrealCoordinateSpace.GetInverse().Quaternion() * quaternion;
    return worker::List<float>{rotatedQuaternion.W, rotatedQuaternion.X, rotatedQuaternion.Y, rotatedQuaternion.Z};
}                                                                                                  
                                                                                                   
inline FQuat ToUnrealRotation(const worker::List<float>& quaterion)                                         
{                                                                                                  
    FQuat result{ quaterion[1], quaterion[2], quaterion[3], quaterion[0] };
    return SpatialToUnrealCoordinateSpace.Quaternion() * result;
}                                                                                                  