// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RpgDemo.h"
#include "Engine.h"
#include "RpgDemoGameMode.h"
#include "RpgDemoPlayerController.h"
#include <improbable/player/heartbeat.h>
#include <improbable/player/heartbeat_receiver.h>
#include <improbable/common/transform.h>
#include "improbable/standard_library.h"


#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

namespace {
worker::Entity GetPlayerEntityTemplate()
{
    const improbable::math::Coordinates initialPosition{ 1.0, 20.0, 0.0 };
    const worker::List<float> initialRoation{ 1.0f, 0.0f, 0.0f, 0.0f };

    const improbable::WorkerAttributeSet unrealWorkerAttributeSet{ {worker::Option<std::string>{"UnrealWorker"}} };
    const improbable::WorkerAttributeSet unrealClientAttributeSet{ {worker::Option<std::string>{"UnrealClient"}} };

    const improbable::WorkerRequirementSet workerRequirementSet{ {unrealWorkerAttributeSet} };
    const improbable::WorkerRequirementSet clientRequirementSet{ {unrealClientAttributeSet} };
    const improbable::WorkerRequirementSet globalRequirmentSet{ {unrealClientAttributeSet, unrealWorkerAttributeSet} };

    worker::Map<std::uint32_t, improbable::WorkerRequirementSet> componentAuthority;

    componentAuthority.emplace(improbable::common::Transform::ComponentId, clientRequirementSet);
    componentAuthority.emplace(improbable::player::Heartbeat::ComponentId, clientRequirementSet);
    componentAuthority.emplace(improbable::player::HeartbeatReceiver::ComponentId, workerRequirementSet);

    const improbable::ComponentAcl componentAcl(componentAuthority);

    worker::Entity playerTempalte;
    playerTempalte.Add<improbable::common::Transform>(improbable::common::Transform::Data{ initialPosition, initialRoation });
    playerTempalte.Add<improbable::player::Heartbeat>(improbable::player::Heartbeat::Data{});
    playerTempalte.Add<improbable::player::HeartbeatReceiver>(improbable::player::HeartbeatReceiver::Data{});
    playerTempalte.Add<improbable::EntityAcl>(improbable::EntityAcl::Data{globalRequirmentSet, componentAcl});
    return playerTempalte;
}
}  // ::

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

void ARpgDemoGameMode::SpawnPlayer()
{
	auto workerConnection = WorkerConnection();

    auto& connection = workerConnection->GetConnection();
    auto& view = workerConnection->GetView();

    const std::uint32_t timeoutMillis = 500;
    const std::string entityType = "Player";

    const auto entityIdReservationRequestId = connection.SendReserveEntityIdRequest(timeoutMillis);

	UE_LOG(LogTemp, Warning, TEXT("Reserve Request sent"))

    view.OnReserveEntityIdResponse([&connection, entityIdReservationRequestId, entityType, timeoutMillis](const worker::ReserveEntityIdResponseOp& op)
    {
		UE_LOG(LogTemp, Warning, TEXT("OnReserveEntityIdResponse"))
        if (op.RequestId == entityIdReservationRequestId && op.StatusCode == worker::StatusCode::kSuccess)
        {
			UE_LOG(LogTemp, Warning, TEXT("OnReserveEntityIdResponse SUCCESS"))
            connection.SendCreateEntityRequest(GetPlayerEntityTemplate(), entityType, op.EntityId, timeoutMillis);
        } 
    });
}

void ARpgDemoGameMode::RegisterEntityBlueprints()
{
	auto workerConnection = WorkerConnection();

	using namespace improbable::unreal::entity_spawning;
	Spawner.Reset(
		new FEntitySpawner(workerConnection->GetConnection(), workerConnection->GetView(), GetWorld()));
	TArray<UObject*> assets;
	if (EngineUtils::FindOrLoadAssetsByPath(TEXT(ENTITY_BLUEPRINTS_FOLDER), assets,
		EngineUtils::ATL_Class))
	{
		for (auto asset : assets)
		{
			UBlueprintGeneratedClass* blueprintGeneratedClass =
				Cast<UBlueprintGeneratedClass>(asset);
			if (blueprintGeneratedClass != nullptr)
			{
				FString blueprintName = blueprintGeneratedClass->GetName().LeftChop(
					2);  // generated blueprint class names end with "_C"
				UE_LOG(LogTemp, Warning,
					TEXT("Registering blueprint in entity spawner with name: %s"),
					*blueprintName)
					Spawner->RegisterPrefabName(blueprintName, blueprintGeneratedClass);
			}
			else
			{
				UE_LOG(
					LogTemp, Warning,
					TEXT("Found asset in the EntityBlueprints folder which is not a blueprint: %s"),
					*(asset->GetFullName()))
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No assets found in EntityBlueprints folder."))
	}
}