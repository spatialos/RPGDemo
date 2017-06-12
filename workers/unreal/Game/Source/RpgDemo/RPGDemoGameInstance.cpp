// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "RPGDemoGameInstance.h"
#include "SimpleEntitySpawnerBlock.h"
#include "EntityRegistry.h"
#include "improbable/Generated/cpp/unreal/EntityPipeline.h"
#include "improbable/Generated/cpp/unreal/CallbackDispatcher.h"

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

URPGDemoGameInstance::URPGDemoGameInstance()
: SpatialOSInstance(), EntitySpawner(nullptr), MetricsReporterHandle()
{
}

URPGDemoGameInstance::~URPGDemoGameInstance()
{
}

void URPGDemoGameInstance::Init()
{
    UGameInstance::Init();

    SpatialOSInstance = NewObject<USpatialOS>(this);

    SpatialOSInstance->OnConnectedDelegate.AddDynamic(this,
                                                      &URPGDemoGameInstance::OnSpatialOsConnected);
    SpatialOSInstance->OnDisconnectedDelegate.AddDynamic(
        this, &URPGDemoGameInstance::OnSpatialOsDisconnected);

	EntityRegistry = NewObject<UEntityRegistry>(this);
	EntityPipeline = NewObject<UEntityPipeline>(this);
	CallbackDispatcher = NewObject<UCallbackDispatcher>(this);
}

void URPGDemoGameInstance::Shutdown()
{
    UGameInstance::Shutdown();

    SpatialOSInstance->OnConnectedDelegate.RemoveDynamic(
        this, &URPGDemoGameInstance::OnSpatialOsConnected);
    SpatialOSInstance->OnDisconnectedDelegate.RemoveDynamic(
        this, &URPGDemoGameInstance::OnSpatialOsDisconnected);
}

void URPGDemoGameInstance::ProcessEvents()
{
	if (SpatialOSInstance != nullptr && EntityPipeline != nullptr)
	{
		SpatialOSInstance->ProcessEvents();
		EntityPipeline->ProcessOps(SpatialOSInstance->GetView(), SpatialOSInstance->GetConnection(), GetWorld());
	}
}

USpatialOS* URPGDemoGameInstance::GetSpatialOS()
{
    return SpatialOSInstance;
}

improbable::unreal::entity_spawning::FEntitySpawner* URPGDemoGameInstance::GetEntitySpawner()
{
    return EntitySpawner.Get();
}

void URPGDemoGameInstance::OnSpatialOsConnected()
{
    using namespace improbable::unreal::entity_spawning;
    EntitySpawner.Reset(new FEntitySpawner(SpatialOSInstance->GetConnection(),
                                           SpatialOSInstance->GetView(), GetWorld()));

    TArray<FString> BlueprintPaths;
    BlueprintPaths.Add(TEXT(ENTITY_BLUEPRINTS_FOLDER));

    EntitySpawner->RegisterEntityBlueprints(BlueprintPaths);

	auto Block = NewObject<USimpleEntitySpawnerBlock>();
	Block->Init(EntityRegistry);
	EntityPipeline->AddBlock(Block);

	CallbackDispatcher->Init(SpatialOSInstance->GetView());

	EntityRegistry->RegisterEntityBlueprints(BlueprintPaths);
	EntityPipeline->Init(SpatialOSInstance->GetView(), CallbackDispatcher);

    constexpr auto ShouldTimerLoop = true;
    constexpr auto InitialDelay = 2.0f;
    constexpr auto LoopDelay = 2.0f;

    auto MetricsDelegate = FTimerDelegate::CreateLambda([this]() {
        auto Connection = SpatialOSInstance->GetConnection().Pin();

        if (Connection.IsValid())
        {
            Connection->SendMetrics(SpatialOSInstance->GetMetrics());
        }
    });

    GetWorld()->GetTimerManager().SetTimer(MetricsReporterHandle, MetricsDelegate, LoopDelay,
                                           ShouldTimerLoop, InitialDelay);
}

void URPGDemoGameInstance::OnSpatialOsDisconnected()
{
    EntitySpawner.Reset(nullptr);
    GetWorld()->GetTimerManager().ClearTimer(MetricsReporterHandle);
}
