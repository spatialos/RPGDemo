
#pragma once

#include "Core.h"

#include "improbable/corelib/math/quaternion.h"
#include "improbable/corelibrary/math/fixed_point_vector3.h"
#include "improbable/corelibrary/math/quaternion32.h"
#include "improbable/math/vector3d.h"

extern const FRotator UnityToUnrealCoordinateSpace;
constexpr double UnityToUnrealScale = 100.0;

constexpr int BITS_FOR_FRACTIONAL_PART = 12;
constexpr double FIXED_POINT_TO_DOUBLE_FACTOR = 1 << BITS_FOR_FRACTIONAL_PART;

inline int64_t ToFixedPoint(double d)
{
  return static_cast<int64_t>(d * FIXED_POINT_TO_DOUBLE_FACTOR);
}

inline double ToDouble(int64_t fixedPoint)
{
  return fixedPoint / FIXED_POINT_TO_DOUBLE_FACTOR;
}

constexpr float ROOT_TWO = 1.4142135623730951f;           // sqrt(2)
constexpr float ONE_OVER_ROOT_TWO = 0.7071067811865475f;  // 1/sqrt(2)
constexpr uint32_t MAX_INT = 0x3FE;
constexpr float MAX_INT_AS_FLOAT = static_cast<float>(MAX_INT);
constexpr int BITS_PER_COMPONENT = 10;
constexpr int BITS_FOR_LARGEST_COMPONENT_INDEX = 2;

inline uint32_t To10Bits(float component)
{
  float mappedComponent = FMath::Clamp(0.5f + (ONE_OVER_ROOT_TWO * component), 0.0f, 1.0f);
  return static_cast<uint32_t>(FMath::FloorToInt(mappedComponent * MAX_INT_AS_FLOAT));
}

inline float ToFloatComponent(uint32_t component)
{
  float mappedFloat = static_cast<float>(component) / MAX_INT_AS_FLOAT;
  return (mappedFloat - 0.5f) * ROOT_TWO;
}

inline void ToComponents(const improbable::corelibrary::math::Quaternion32& quaternion32,
                         float components[4])
{
  int indexOfLargestComponent =
      quaternion32.quaternion() >> (32 - BITS_FOR_LARGEST_COMPONENT_INDEX);
  float commponentsSquareSum = 0.0f;

  int bitsToShift = 32 - BITS_FOR_LARGEST_COMPONENT_INDEX - BITS_PER_COMPONENT;
  for (int i = 0; i < 4; ++i)
  {
    if (i == indexOfLargestComponent)
    {
      continue;
    }
    int intergerRepresentation = (quaternion32.quaternion() >> bitsToShift) & MAX_INT;
    components[i] = ToFloatComponent(intergerRepresentation);
    commponentsSquareSum += components[i] * components[i];
    bitsToShift -= BITS_PER_COMPONENT;
  }
  components[indexOfLargestComponent] =
      FMath::Clamp(FMath::Sqrt(1.0f - commponentsSquareSum), 0.0f, 1.0f);
}

inline improbable::math::Vector3d
ToNativeVector(const improbable::corelibrary::math::FixedPointVector3& fixedPoint)
{
  return improbable::math::Vector3d(ToDouble(fixedPoint.fixed_point_values()[0]),
                                    ToDouble(fixedPoint.fixed_point_values()[1]),
                                    ToDouble(fixedPoint.fixed_point_values()[2]));
}

inline FVector ToUnrealVector(const improbable::corelibrary::math::FixedPointVector3& fixedPoint)
{
  auto result = FVector(static_cast<float>(ToDouble(fixedPoint.fixed_point_values()[0])),
                        static_cast<float>(ToDouble(fixedPoint.fixed_point_values()[1])),
                        static_cast<float>(ToDouble(fixedPoint.fixed_point_values()[2])));
  result = UnityToUnrealCoordinateSpace.RotateVector(result);
  result *= UnityToUnrealScale;

  return result;
}

inline improbable::corelibrary::math::FixedPointVector3 ToFixedPoint(float x, float y, float z)
{
  return improbable::corelibrary::math::FixedPointVector3{
      worker::List<int64_t>{ToFixedPoint(x), ToFixedPoint(y), ToFixedPoint(z)}};
}

inline improbable::corelibrary::math::FixedPointVector3 ToFixedPoint(const FVector& pos)
{
  return ToFixedPoint(pos.X, pos.Y, pos.Z);
}

inline FVector UnrealToSpatialPosition(const FVector& pos)
{
  return UnityToUnrealCoordinateSpace.GetInverse().RotateVector(pos) / UnityToUnrealScale;
}

inline bool IsIdentity(float x, float y, float z, float w)
{
  return w == 1.0f || w == -1.0f;
}

inline bool IsNormalized(float x, float y, float z, float w, float tolerance)
{
  return w * w + x * x + y * y + z * z - 1.0f < tolerance;
}

inline int GetLargestComponentIndex(float components[4])
{
  int largestComponentIndex = 0;
  int largestAbsComponentValue = FMath::Abs(components[0]);
  for (int i = 1; i < 4; ++i)
  {
    float absComponentValue = FMath::Abs(components[i]);
    if (absComponentValue > largestAbsComponentValue)
    {
      largestComponentIndex = i;
      largestAbsComponentValue = absComponentValue;
    }
  }
  return largestComponentIndex;
}

inline uint32_t ToQuaternion32(float x, float y, float z, float w)
{
  if (!IsNormalized(x, y, z, w, 0.001f))
  {
    UE_LOG(LogTemp, Fatal, TEXT("Can not serialize non-unit quaternion: (%f %f %f, w=%f)"), x, y, z,
           w);
  }

  // Check for the identity
  if (IsIdentity(x, y, z, w))
  {
    return 0x3FF;
  }

  float components[4] = {w, x, y, z};
  int largestComponentIndex = GetLargestComponentIndex(components);

  if (components[largestComponentIndex] < 0)
  {
    for (int i = 0; i < 4; ++i)
    {
      components[i] = -components[i];
    }
  }

  // first two bits store the index of the largest number (which will not be serialized)
  uint32_t quaternion = static_cast<uint32_t>(largestComponentIndex)
      << (32 - BITS_FOR_LARGEST_COMPONENT_INDEX);

  int shiftBitsBy = 32 - BITS_FOR_LARGEST_COMPONENT_INDEX - BITS_PER_COMPONENT;
  for (int i = 0; i < 4; ++i)
  {
    if (i == largestComponentIndex)
    {
      continue;
    }
    quaternion |= To10Bits(components[i]) << shiftBitsBy;
    shiftBitsBy -= BITS_PER_COMPONENT;
  }
  return quaternion;
}

inline uint32_t ToQuaternion32(const FQuat& rotation)
{
  return ToQuaternion32(rotation.X, rotation.Y, rotation.Z, rotation.W);
}

inline FQuat UnrealToSpatialQuaternion(const FQuat& rotation)
{
  auto rotator = UnityToUnrealCoordinateSpace.GetInverse().Quaternion();
  return rotation * rotator;
}

inline improbable::corelib::math::Quaternion
ToNativeQuaternion(const improbable::corelibrary::math::Quaternion32& quaternion)
{
  if ((quaternion.quaternion() & 0x3FF) == 0x3FF)
  {
    return improbable::corelib::math::Quaternion(1, 0, 0, 0);
  }
  float components[4];
  ToComponents(quaternion, components);
  return improbable::corelib::math::Quaternion(components[0], components[1], components[2],
                                               components[3]);
}

inline FQuat ToUnrealQuaternion(const uint32_t quaternion)
{
  if ((quaternion & 0x3FF) == 0x3FF)
  {
    return FQuat::Identity;
  }

  float components[4];
  ToComponents(quaternion, components);

  // N.B. w is stored natively as the last component
  FQuat result(components[1], components[2], components[3], components[0]);
  result = UnityToUnrealCoordinateSpace.Quaternion() * result;

  return result;
}

// inline improbable::corelibrary::math::Quaternion32 ToUnityQuaternion(const FQuat& unrealQuat)
//{
//	auto rotator = UnityToUnrealCoordinateSpace.GetInverse().Quaternion();
//	auto rotatedQuat = unrealQuat * rotator;
//	float components[4];
//	components[0] = rotatedQuat.X;
//	components[1] = rotatedQuat.Y;
//	components[2] = rotatedQuat.Z;
//	components[3] = rotatedQuat.W;
//
//	// N.B. w is stored natively as the last component
//	auto unityQuat = ToQuaternion32(components[3], components[0], components[1], components[2]);
//	return unityQuat;
//}