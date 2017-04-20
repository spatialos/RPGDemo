// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "RPGDemoGameInstance.h"

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

URPGDemoGameInstance::URPGDemoGameInstance()
: SpatialOSInstance()
, EntitySpawner(nullptr)
, OnConnectedDelegateHandle()
, OnDisconnectedDelegateHandle()
{
    SpatialOSInstance = NewObject<USpatialOS>();

    OnConnectedDelegateHandle =
        SpatialOSInstance->OnConnectedDelegate.AddLambda([this](UWorld* InWorld) {
            using namespace improbable::unreal::entity_spawning;
            EntitySpawner.Reset(new FEntitySpawner(SpatialOSInstance->GetConnection(),
                                                   SpatialOSInstance->GetView(), InWorld));

            TArray<FString> BlueprintPaths;
            BlueprintPaths.Add(TEXT(ENTITY_BLUEPRINTS_FOLDER));

            EntitySpawner->RegisterEntityBlueprints(BlueprintPaths);
        });

    OnDisconnectedDelegateHandle = SpatialOSInstance->OnDisconnectedDelegate.AddLambda(
        [this]() { EntitySpawner.Reset(nullptr); });
}

URPGDemoGameInstance::~URPGDemoGameInstance()
{
    SpatialOSInstance->OnConnectedDelegate.Remove(OnConnectedDelegateHandle);
    SpatialOSInstance->OnConnectedDelegate.Remove(OnDisconnectedDelegateHandle);
}

USpatialOS* URPGDemoGameInstance::GetSpatialOS()
{
    return SpatialOSInstance;
}

improbable::unreal::entity_spawning::FEntitySpawner* URPGDemoGameInstance::GetEntitySpawner()
{
    return EntitySpawner.Get();
}
