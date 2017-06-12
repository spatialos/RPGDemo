// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Engine/GameInstance.h"
#include "EntitySpawner.h"
#include "SpatialOS.h"
#include "RPGDemoGameInstance.generated.h"

class UEntityPipeline;
class UEntityRegistry;
class UCallbackDispatcher;

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

    virtual void Init() override;
    virtual void Shutdown() override;

	void ProcessEvents();

    UFUNCTION(BlueprintCallable, Category = "SpatialOS")
    USpatialOS* GetSpatialOS();

    improbable::unreal::entity_spawning::FEntitySpawner* GetEntitySpawner();

	UPROPERTY()
		UCallbackDispatcher* CallbackDispatcher;

  private:
    UPROPERTY()
    USpatialOS* SpatialOSInstance;

	UPROPERTY()
	UEntityPipeline* EntityPipeline;

	UPROPERTY()
	UEntityRegistry* EntityRegistry;

    UFUNCTION()
    void OnSpatialOsConnected();

    UFUNCTION()
    void OnSpatialOsDisconnected();

    TUniquePtr<improbable::unreal::entity_spawning::FEntitySpawner> EntitySpawner;

    FTimerHandle MetricsReporterHandle;
};
