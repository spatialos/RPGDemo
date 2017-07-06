// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "SpatialOSConversionFunctionLibrary.h"
#include "RPGDemoGameInstance.h"
#include "RpgDemoGameMode.h"
#include "RpgDemoPlayerController.h"
#include "SpatialOSWorkerConfigurationData.h"
#include "WorkerConnection.h"
#include "improbable/standard_library.h"
#include <improbable/player/heartbeat.h>
#include <improbable/spawner/spawner.h>

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

using namespace improbable;
using namespace improbable::unreal::core;

ARpgDemoGameMode::ARpgDemoGameMode()
: entityQueryCallback(-1), WorkerTypeOverride(""), WorkerIdOverride(""), UseExternalIp(false)
{
    PrimaryActorTick.bCanEverTick = true;

    // Set the default player controller class
    PlayerControllerClass = ARpgDemoPlayerController::StaticClass();

    // Don't spawn players automatically
    bStartPlayersAsSpectators = true;

    // No need for default pawn class
    DefaultPawnClass = nullptr;

    UnbindEntityQueryDelegate.BindUObject(this, &ARpgDemoGameMode::UnbindEntityQueryCallback);
}

FString ARpgDemoGameMode::GetSpatialOsWorkerType() const
{
    auto SpatialOS = GetSpatialOS();
    if (SpatialOS != nullptr)
    {
        return SpatialOS->GetWorkerConfiguration().GetWorkerType();
    }
    return "";
}

UEntityTemplate* ARpgDemoGameMode::CreatePlayerEntityTemplate(FString clientWorkerId,
                                                              const FVector& position)
{
    const auto& spatialOsPosition =
		USpatialOSConversionFunctionLibrary::UnrealCoordinatesToSpatialOsCoordinates(position);
    const Coordinates initialPosition{spatialOsPosition.X, spatialOsPosition.Y,
                                      spatialOsPosition.Z};
    const worker::List<float> initialRotation{1.0f, 0.0f, 0.0f, 0.0f};

    const WorkerAttributeSet unrealWorkerAttributeSet{worker::List<std::string>{"UnrealWorker"}};

    const std::string clientWorkerIdString = TCHAR_TO_UTF8(*clientWorkerId);
    const std::string clientAttribute = "workerId:" + clientWorkerIdString;
    UE_LOG(LogTemp, Warning, TEXT("Making ourselves authoritative over Player Transform and "
                                  "HeartbeatReceiver with worker ID %s"),

           *FString(clientAttribute.c_str()))
    const WorkerAttributeSet ownUnrealClientAttributeSet{
        worker::List<std::string>{clientAttribute}};
    const WorkerAttributeSet allUnrealClientsAttributeSet{
        worker::List<std::string>{"UnrealClient"}};

    const WorkerRequirementSet workerRequirementSet{{unrealWorkerAttributeSet}};
    const WorkerRequirementSet ownClientRequirementSet{{ownUnrealClientAttributeSet}};
    const WorkerRequirementSet globalRequirementSet{
        {allUnrealClientsAttributeSet, unrealWorkerAttributeSet}};

    worker::Map<std::uint32_t, WorkerRequirementSet> componentAuthority;

    componentAuthority.emplace(Position::ComponentId, ownClientRequirementSet);
    componentAuthority.emplace(player::HeartbeatReceiver::ComponentId, ownClientRequirementSet);
    componentAuthority.emplace(player::HeartbeatSender::ComponentId, workerRequirementSet);

    worker::Entity playerTemplate;
    playerTemplate.Add<Position>(Position::Data{initialPosition});
    playerTemplate.Add<player::HeartbeatSender>(player::HeartbeatSender::Data{});
    playerTemplate.Add<player::HeartbeatReceiver>(player::HeartbeatReceiver::Data{});
    playerTemplate.Add<EntityAcl>(EntityAcl::Data{globalRequirementSet, componentAuthority});
    return NewObject<UEntityTemplate>(this, UEntityTemplate::StaticClass())->Init(playerTemplate);
}

void ARpgDemoGameMode::GetSpawnerEntityId(const FGetSpawnerEntityIdResultDelegate& callback,
                                          int timeoutMs)
{
    auto SpatialOS = GetSpatialOS();
    if (SpatialOS != nullptr)
    {
        auto LockedConnection = SpatialOS->GetConnection().Pin();

        if (LockedConnection.IsValid())
        {
            auto LockedDispatcher = SpatialOS->GetView().Pin();

            if (LockedDispatcher.IsValid())
            {
                GetSpawnerEntityIdResultCallback = new FGetSpawnerEntityIdResultDelegate(callback);
                const worker::query::EntityQuery& entity_query = {
                    worker::query::ComponentConstraint{spawner::Spawner::ComponentId},
                    worker::query::SnapshotResultType{}};

                auto requestId = LockedConnection->SendEntityQueryRequest(
                    entity_query, static_cast<std::uint32_t>(timeoutMs));
                entityQueryCallback = LockedDispatcher->OnEntityQueryResponse([this, requestId](
                    const worker::EntityQueryResponseOp& op) {
                    if (op.RequestId != requestId)
                    {
                        return;
                    }
                    if (!GetSpawnerEntityIdResultCallback->IsBound())
                    {
                        UE_LOG(LogTemp, Warning,
                               TEXT("GetSpawnerEntityIdResultCallback is unbound"))
                    }
                    if (op.StatusCode != worker::StatusCode::kSuccess)
                    {
                        std::string errorMessage = "Could not find spawner entity: " + op.Message;
                        GetSpawnerEntityIdResultCallback->ExecuteIfBound(
                            false, FString(errorMessage.c_str()), FEntityId());
                        return;
                    }
                    if (op.ResultCount == 0)
                    {
                        std::string errorMessage = "Query returned 0 spawner entities";
                        GetSpawnerEntityIdResultCallback->ExecuteIfBound(
                            false, FString(errorMessage.c_str()), FEntityId());
                        return;
                    }
                    GetSpawnerEntityIdResultCallback->ExecuteIfBound(
                        true, FString(), FEntityId(op.Result.begin()->first));
                    GetWorldTimerManager().SetTimerForNextTick(UnbindEntityQueryDelegate);
                    return;
                });
            }
        }
    }
}

void ARpgDemoGameMode::StartPlay()
{
    AGameModeBase::StartPlay();

    auto SpatialOS = GetSpatialOS();
    if (SpatialOS != nullptr)
    {
        SpatialOS->OnConnectedDelegate.AddDynamic(this, &ARpgDemoGameMode::OnSpatialOsConnected);
        SpatialOS->OnConnectionFailedDelegate.AddDynamic(
            this, &ARpgDemoGameMode::OnSpatialOsFailedToConnect);
        SpatialOS->OnDisconnectedDelegate.AddDynamic(this,
                                                     &ARpgDemoGameMode::OnSpatialOsDisconnected);
        UE_LOG(LogSpatialOS, Display, TEXT("Startplay called to SpatialOS"))

        auto workerConfig = FSOSWorkerConfigurationData();

        workerConfig.Networking.UseExternalIp = UseExternalIp;

        if (!WorkerTypeOverride.IsEmpty())
        {
            workerConfig.SpatialOSApplication.WorkerPlatform = WorkerTypeOverride;
        }

        if (!WorkerIdOverride.IsEmpty())
        {
            workerConfig.SpatialOSApplication.WorkerId = WorkerIdOverride;
        }

        SpatialOS->ApplyConfiguration(workerConfig);
        SpatialOS->Connect();
    }
}

void ARpgDemoGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AGameModeBase::EndPlay(EndPlayReason);

	UnbindEntityQueryCallback();

    auto SpatialOS = GetSpatialOS();
    if (SpatialOS != nullptr)
    {
        SpatialOS->Disconnect();

        SpatialOS->OnConnectedDelegate.RemoveDynamic(this, &ARpgDemoGameMode::OnSpatialOsConnected);
        SpatialOS->OnConnectionFailedDelegate.RemoveDynamic(
            this, &ARpgDemoGameMode::OnSpatialOsFailedToConnect);
        SpatialOS->OnDisconnectedDelegate.RemoveDynamic(this,
                                                        &ARpgDemoGameMode::OnSpatialOsDisconnected);
    }
}

void ARpgDemoGameMode::Tick(float DeltaTime)
{
    AGameModeBase::Tick(DeltaTime);

	auto GameInstance = Cast<URPGDemoGameInstance>(GetWorld()->GetGameInstance());

	if (GameInstance != nullptr)
	{
		GameInstance->ProcessOps();
	}
}

bool ARpgDemoGameMode::IsConnectedToSpatialOs() const
{
    auto SpatialOS = GetSpatialOS();
    if (SpatialOS != nullptr)
    {
        return SpatialOS->IsConnected();
    }
    return false;
}

UCommander* ARpgDemoGameMode::SendWorkerCommand()
{
    if (Commander == nullptr)
    {
        auto SpatialOS = GetSpatialOS();
        if (SpatialOS != nullptr)
        {
            Commander = NewObject<UCommander>(this, UCommander::StaticClass())
                            ->Init(nullptr, SpatialOS->GetConnection(), SpatialOS->GetView());
        }
    }
    return Commander;
}

void ARpgDemoGameMode::UnbindEntityQueryCallback()
{
    if (entityQueryCallback != -1)
    {
        auto SpatialOS = GetSpatialOS();
        if (SpatialOS != nullptr)
        {
            auto LockedDispatcher = SpatialOS->GetView().Pin();

            if (LockedDispatcher.IsValid())
            {
                LockedDispatcher->Remove(entityQueryCallback);
                entityQueryCallback = -1;
            }
        }
    }
}

USpatialOS* ARpgDemoGameMode::GetSpatialOS() const
{
    auto gameInstance = Cast<URPGDemoGameInstance>(GetWorld()->GetGameInstance());

    if (gameInstance != nullptr)
    {
        return gameInstance->GetSpatialOS();
    }

    return nullptr;
}