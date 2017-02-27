// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RpgDemo.h"
#include "RpgDemoGameMode.h"
#include "RpgDemoPlayerController.h"
#include <improbable/player/heartbeat.h>
#include <improbable/common/transform.h>
#include "improbable/standard_library.h"
#include "ConversionsFunctionLibrary.h"

ARpgDemoGameMode* ARpgDemoGameMode::Instance;

ARpgDemoGameMode::ARpgDemoGameMode()
{
    // Set the default player controller class
    PlayerControllerClass = ARpgDemoPlayerController::StaticClass();

    // Don't spawn players automatically
    bStartPlayersAsSpectators = true;

    // No need for default pawn class
    DefaultPawnClass = nullptr;

    Instance = this;
}

ARpgDemoGameMode::~ARpgDemoGameMode()
{
    Instance = nullptr;
}

void ARpgDemoGameMode::Tick(float DeltaTime)
{
	ASpatialOSGameMode::Tick(DeltaTime);
}

UEntityTemplate* ARpgDemoGameMode::CreatePlayerEntityTemplate(FString clientWorkerId, const FVector& position)
{
	const auto& spatialOsPosition = UConversionsFunctionLibrary::UnrealCoordinatesToSpatialOsCoordinates(position);
    const improbable::math::Coordinates initialPosition{ spatialOsPosition.X, spatialOsPosition.Y, spatialOsPosition.Z };
    const worker::List<float> initialRoation{ 1.0f, 0.0f, 0.0f, 0.0f };

    const improbable::WorkerAttributeSet unrealWorkerAttributeSet{ {worker::Option<std::string>{"UnrealWorker"}} };
	const std::string clientWorkerIdString = TCHAR_TO_UTF8(*clientWorkerId);
	const std::string clientAttribute = "workerId:" + clientWorkerIdString;
	UE_LOG(LogTemp, Warning,
		TEXT("Making ourselves authoritative over Player Transform and HeartbeatReceiver with worker ID %s"),
		*FString(clientAttribute.c_str()))
    const improbable::WorkerAttributeSet ownUnrealClientAttributeSet{ {worker::Option<std::string>{clientAttribute}} };
    const improbable::WorkerAttributeSet allUnrealClientsAttributeSet{ {worker::Option<std::string>{"UnrealClient"}} };

    const improbable::WorkerRequirementSet workerRequirementSet{ {unrealWorkerAttributeSet} };
    const improbable::WorkerRequirementSet ownClientRequirementSet{ {ownUnrealClientAttributeSet} };
    const improbable::WorkerRequirementSet globalRequirementSet{ {allUnrealClientsAttributeSet, unrealWorkerAttributeSet} };

    worker::Map<std::uint32_t, improbable::WorkerRequirementSet> componentAuthority;

    componentAuthority.emplace(improbable::common::Transform::ComponentId, ownClientRequirementSet);
    componentAuthority.emplace(improbable::player::HeartbeatReceiver::ComponentId, ownClientRequirementSet);
    componentAuthority.emplace(improbable::player::HeartbeatSender::ComponentId, workerRequirementSet);

    const improbable::ComponentAcl componentAcl(componentAuthority);

    worker::Entity playerTemplate;
    playerTemplate.Add<improbable::common::Transform>(improbable::common::Transform::Data{ initialPosition, initialRoation });
    playerTemplate.Add<improbable::player::HeartbeatSender>(improbable::player::HeartbeatSender::Data{});
    playerTemplate.Add<improbable::player::HeartbeatReceiver>(improbable::player::HeartbeatReceiver::Data{});
    playerTemplate.Add<improbable::EntityAcl>(improbable::EntityAcl::Data{globalRequirementSet, componentAcl});
    return NewObject<UEntityTemplate>(this, UEntityTemplate::StaticClass())->Init(playerTemplate);
}
