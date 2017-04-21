// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "RPGDemoGameInstance.h"

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

URPGDemoGameInstance::URPGDemoGameInstance()
: SpatialOSInstance()
, EntitySpawner(nullptr)
, OnConnectedDelegateHandle()
, OnDisconnectedDelegateHandle()
, MetricsReporterHandle()
{
  
}

URPGDemoGameInstance::~URPGDemoGameInstance()
{

}

void URPGDemoGameInstance::Init()
{
	UGameInstance::Init();

	SpatialOSInstance = NewObject<USpatialOS>(this);

	OnConnectedDelegateHandle =
		SpatialOSInstance->OnConnectedDelegate.AddLambda([this]() {
		using namespace improbable::unreal::entity_spawning;
		EntitySpawner.Reset(new FEntitySpawner(SpatialOSInstance->GetConnection(),
			SpatialOSInstance->GetView(), GetWorld()));

		TArray<FString> BlueprintPaths;
		BlueprintPaths.Add(TEXT(ENTITY_BLUEPRINTS_FOLDER));

		EntitySpawner->RegisterEntityBlueprints(BlueprintPaths);

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
	});

	OnDisconnectedDelegateHandle = SpatialOSInstance->OnDisconnectedDelegate.AddLambda([this]() {
		EntitySpawner.Reset(nullptr);
		GetWorld()->GetTimerManager().ClearTimer(MetricsReporterHandle);
	});
}

void URPGDemoGameInstance::Shutdown()
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
