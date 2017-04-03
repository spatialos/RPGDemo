// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Engine/GameInstance.h"
#include "SpatialOS.h"
#include "RPGDemoGameInstance.generated.h"

/**
 *
 */
UCLASS()
class RPGDEMO_API URPGDemoGameInstance : public UGameInstance
{
    GENERATED_BODY()
  public:
    URPGDemoGameInstance();
    ~URPGDemoGameInstance();

    improbable::unreal::core::FSpatialOS& GetSpatialOS();

  private:
    improbable::unreal::core::FSpatialOS SpatialOSInstance;
};
