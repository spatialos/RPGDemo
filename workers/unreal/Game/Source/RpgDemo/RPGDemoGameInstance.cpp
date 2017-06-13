// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "RPGDemoGameInstance.h"
#include "SimpleEntitySpawnerBlock.h"
#include "EntityRegistry.h"
#include "SpatialOS.h"

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

URPGDemoGameInstance::URPGDemoGameInstance()
: SpatialOSInstance(), MetricsReporterHandle()
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
	if (SpatialOSInstance != nullptr && SpatialOSInstance->GetEntityPipeline() != nullptr)
	{
		SpatialOSInstance->ProcessEvents();
		SpatialOSInstance->GetEntityPipeline()->ProcessOps(SpatialOSInstance->GetView(), SpatialOSInstance->GetConnection(), GetWorld());
	}
}

USpatialOS* URPGDemoGameInstance::GetSpatialOS()
{
    return SpatialOSInstance;
}

UEntityRegistry* URPGDemoGameInstance::GetEntityRegistry()
{
    return EntityRegistry;
}

void URPGDemoGameInstance::OnSpatialOsConnected()
{
	auto EntitySpawnerBlock = NewObject<USimpleEntitySpawnerBlock>();
	EntitySpawnerBlock->Init(EntityRegistry);
	SpatialOSInstance->GetEntityPipeline()->AddBlock(EntitySpawnerBlock);

	TArray<FString> BlueprintPaths;
	BlueprintPaths.Add(TEXT(ENTITY_BLUEPRINTS_FOLDER));

	EntityRegistry->RegisterEntityBlueprints(BlueprintPaths);
		
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
    GetWorld()->GetTimerManager().ClearTimer(MetricsReporterHandle);
}
