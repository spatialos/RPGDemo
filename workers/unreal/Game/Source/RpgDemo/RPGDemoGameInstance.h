// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Engine/GameInstance.h"
#include "EntitySpawner.h"
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

    UFUNCTION(BlueprintCallable, Category = "SpatialOS")
    USpatialOS* GetSpatialOS();
    improbable::unreal::entity_spawning::FEntitySpawner* GetEntitySpawner();

  private:
    UPROPERTY()
    USpatialOS* SpatialOSInstance;

    TUniquePtr<improbable::unreal::entity_spawning::FEntitySpawner> EntitySpawner;

    FDelegateHandle OnConnectedDelegateHandle;
    FDelegateHandle OnDisconnectedDelegateHandle;
};
