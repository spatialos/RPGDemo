// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "improbable/test/test.h"
#include "DamageResponse.generated.h"

/**
 *
 */
UCLASS()
class UNREAL_API UDamageResponse : public UObject
{
    GENERATED_BODY()

  public:
    UDamageResponse();
    UDamageResponse(const improbable::test::DamageResponse& rawDamageResponse);

    UFUNCTION(BlueprintPure, Category = "DamageResponse")
    int GetDealtDamage();

    UFUNCTION(BlueprintCallable, Category = "DamageResponse")
    UDamageResponse* SetDealtDamage(int amount);

    improbable::test::DamageResponse* GetRawDamageResponse();

  private:
    TUniquePtr<improbable::test::DamageResponse> mRawDamageResponse;
};
