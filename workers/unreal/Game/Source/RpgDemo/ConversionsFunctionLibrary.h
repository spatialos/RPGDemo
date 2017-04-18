// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ConversionsFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class RPGDEMO_API UConversionsFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
  public:
    UFUNCTION(BlueprintPure, Category = "Conversions")
    static FRotator GetSpatialOsToUnrealCoordinateSpace();

    UFUNCTION(BlueprintPure, Category = "Conversions")
    static float GetSpatialOsToUnrealScale();

    UFUNCTION(BlueprintPure, Category = "Conversions")
    static FVector UnrealCoordinatesToSpatialOsCoordinates(const FVector& unrealCoordinates);

    UFUNCTION(BlueprintPure, Category = "Conversions")
    static FVector SpatialOsCoordinatesToUnrealCoordinates(const FVector& spatialOsCoordinates);

    UFUNCTION(BlueprintPure, Category = "Conversions")
    static FQuat UnrealRotationToSpatialOsRotation(const FQuat& unrealRotation);

    UFUNCTION(BlueprintPure, Category = "Conversions")
    static FQuat SpatialOsRotationToUnrealRotation(const FQuat& spatialOsRotation);
};
