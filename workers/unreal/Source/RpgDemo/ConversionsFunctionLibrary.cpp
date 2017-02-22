// Fill out your copyright notice in the Description page of Project Settings.

#include "RpgDemo.h"
#include "ConversionsFunctionLibrary.h"

FRotator UConversionsFunctionLibrary::GetSpatialOsToUnrealCoordinateSpace()
{
	return FRotator{ 0.0f, -90.0f, -90.0f };
}

float UConversionsFunctionLibrary::GetSpatialOsToUnrealScale()
{
	return 100.0;
}

FVector UConversionsFunctionLibrary::UnrealCoordinatesToSpatialOsCoordinates(const FVector& unrealCoordinates)
{
	return GetSpatialOsToUnrealCoordinateSpace().GetInverse().RotateVector(unrealCoordinates) / GetSpatialOsToUnrealScale();
}

FVector UConversionsFunctionLibrary::SpatialOsCoordinatesToUnrealCoordinates(const FVector& spatialOsCoordinates)
{
	return GetSpatialOsToUnrealCoordinateSpace().RotateVector(spatialOsCoordinates) * GetSpatialOsToUnrealScale();
}

FQuat UConversionsFunctionLibrary::UnrealRotationToSpatialOsRotation(const FQuat& unrealRotation)
{
	return GetSpatialOsToUnrealCoordinateSpace().GetInverse().Quaternion() * unrealRotation;
}

FQuat UConversionsFunctionLibrary::SpatialOsRotationToUnrealRotation(const FQuat& spatialOsRotation)
{
	return GetSpatialOsToUnrealCoordinateSpace().Quaternion() * spatialOsRotation;
}



