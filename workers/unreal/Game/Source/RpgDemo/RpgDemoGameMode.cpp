// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "ConversionsFunctionLibrary.h"
#include "RPGDemoGameInstance.h"
#include "RpgDemoGameMode.h"
#include "RpgDemoPlayerController.h"
#include "SpatialOSWorkerConfigurationData.h"
#include "WorkerConnection.h"
#include "improbable/standard_library.h"
#include <improbable/common/transform.h>
#include <improbable/player/heartbeat.h>
#include <improbable/spawner/spawner.h>

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

using namespace improbable;
using namespace improbable::unreal::core;

ARpgDemoGameMode::ARpgDemoGameMode()
: entityQueryCallback(-1), WorkerTypeOverride(""), WorkerIdOverride("")
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

ARpgDemoGameMode::~ARpgDemoGameMode()
{
    UnbindEntityQueryCallback();
}

FString ARpgDemoGameMode::GetSpatialOsWorkerType() const
{
    return GetSpatialOS()->GetWorkerConfiguration().GetWorkerType();
}

UEntityTemplate* ARpgDemoGameMode::CreatePlayerEntityTemplate(FString clientWorkerId,
                                                              const FVector& position)
{
    const auto& spatialOsPosition =
        UConversionsFunctionLibrary::UnrealCoordinatesToSpatialOsCoordinates(position);
    const math::Coordinates initialPosition{spatialOsPosition.X, spatialOsPosition.Y,
                                            spatialOsPosition.Z};
    const worker::List<float> initialRoation{1.0f, 0.0f, 0.0f, 0.0f};

    const WorkerAttributeSet unrealWorkerAttributeSet{
        {worker::Option<std::string>{"UnrealWorker"}}};
    const std::string clientWorkerIdString = TCHAR_TO_UTF8(*clientWorkerId);
    const std::string clientAttribute = "workerId:" + clientWorkerIdString;
    UE_LOG(LogTemp, Warning, TEXT("Making ourselves authoritative over Player Transform and "
                                  "HeartbeatReceiver with worker ID %s"),
           *FString(clientAttribute.c_str()))
    const WorkerAttributeSet ownUnrealClientAttributeSet{
        {worker::Option<std::string>{clientAttribute}}};
    const WorkerAttributeSet allUnrealClientsAttributeSet{
        {worker::Option<std::string>{"UnrealClient"}}};

    const WorkerRequirementSet workerRequirementSet{{unrealWorkerAttributeSet}};
    const WorkerRequirementSet ownClientRequirementSet{{ownUnrealClientAttributeSet}};
    const WorkerRequirementSet globalRequirementSet{
        {allUnrealClientsAttributeSet, unrealWorkerAttributeSet}};

    worker::Map<std::uint32_t, WorkerRequirementSet> componentAuthority;

    componentAuthority.emplace(common::Transform::ComponentId, ownClientRequirementSet);
    componentAuthority.emplace(player::HeartbeatReceiver::ComponentId, ownClientRequirementSet);
    componentAuthority.emplace(player::HeartbeatSender::ComponentId, workerRequirementSet);

    const improbable::ComponentAcl componentAcl(componentAuthority);

    worker::Entity playerTemplate;
    playerTemplate.Add<common::Transform>(common::Transform::Data{initialPosition, initialRoation});
    playerTemplate.Add<player::HeartbeatSender>(player::HeartbeatSender::Data{});
    playerTemplate.Add<player::HeartbeatReceiver>(player::HeartbeatReceiver::Data{});
    playerTemplate.Add<EntityAcl>(EntityAcl::Data{globalRequirementSet, componentAcl});
    return NewObject<UEntityTemplate>(this, UEntityTemplate::StaticClass())->Init(playerTemplate);
}

void ARpgDemoGameMode::GetSpawnerEntityId(const FGetSpawnerEntityIdResultDelegate& callback,
                                          int timeoutMs)
{
    auto LockedConnection = GetSpatialOS()->GetConnection().Pin();

    if (LockedConnection.IsValid())
    {
        auto LockedDispatcher = GetSpatialOS()->GetView().Pin();

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
                    UE_LOG(LogTemp, Warning, TEXT("GetSpawnerEntityIdResultCallback is unbound"))
                }
                if (op.StatusCode != worker::StatusCode::kSuccess)
                {
                    std::string errorMessage = "Could not find spawner entity: " + op.Message;
                    GetSpawnerEntityIdResultCallback->ExecuteIfBound(
                        false, FString(errorMessage.c_str()), -1);
                    return;
                }
                if (op.ResultCount == 0)
                {
                    std::string errorMessage = "Query returned 0 spawner entities";
                    GetSpawnerEntityIdResultCallback->ExecuteIfBound(
                        false, FString(errorMessage.c_str()), -1);
                    return;
                }
                GetSpawnerEntityIdResultCallback->ExecuteIfBound(
                    true, FString(), static_cast<int>(op.Result.begin()->first));
                GetWorldTimerManager().SetTimerForNextTick(UnbindEntityQueryDelegate);
                return;
            });
        }
    }
}

void ARpgDemoGameMode::StartPlay()
{
    AGameModeBase::StartPlay();

    GetSpatialOS()->OnConnectedDelegate.AddDynamic(this, &ARpgDemoGameMode::OnSpatialOsConnected);
    GetSpatialOS()->OnConnectionFailedDelegate.AddDynamic(
        this, &ARpgDemoGameMode::OnSpatialOsFailedToConnect);
    GetSpatialOS()->OnDisconnectedDelegate.AddDynamic(this,
                                                      &ARpgDemoGameMode::OnSpatialOsDisconnected);
    UE_LOG(LogSpatialOS, Display, TEXT("Startplay called to SpatialOS"))

    auto workerConfig = FSOSWorkerConfigurationData();

    if (!WorkerTypeOverride.IsEmpty())
    {
        workerConfig.SpatialOSApplication.WorkerPlatform = WorkerTypeOverride;
    }

    if (!WorkerIdOverride.IsEmpty())
    {
        workerConfig.SpatialOSApplication.WorkerId = WorkerIdOverride;
    }

    GetSpatialOS()->ApplyConfiguration(workerConfig);
    GetSpatialOS()->Connect();
}

void ARpgDemoGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AGameModeBase::EndPlay(EndPlayReason);
    GetSpatialOS()->Disconnect();

	GetSpatialOS()->OnConnectedDelegate.RemoveDynamic(this, &ARpgDemoGameMode::OnSpatialOsConnected);
	GetSpatialOS()->OnConnectionFailedDelegate.RemoveDynamic(
		this, &ARpgDemoGameMode::OnSpatialOsFailedToConnect);
	GetSpatialOS()->OnDisconnectedDelegate.RemoveDynamic(this,
		&ARpgDemoGameMode::OnSpatialOsDisconnected);
}

void ARpgDemoGameMode::Tick(float DeltaTime)
{
    AGameModeBase::Tick(DeltaTime);
    GetSpatialOS()->ProcessEvents();
}

bool ARpgDemoGameMode::IsConnectedToSpatialOs() const
{
    return GetSpatialOS()->IsConnected();
}

UCommander* ARpgDemoGameMode::SendWorkerCommand()
{
    if (Commander == nullptr)
    {
        Commander = NewObject<UCommander>(this, UCommander::StaticClass())
                        ->Init(nullptr, GetSpatialOS()->GetConnection(), GetSpatialOS()->GetView());
    }
    return Commander;
}

void ARpgDemoGameMode::UnbindEntityQueryCallback()
{
    if (entityQueryCallback != -1)
    {
        auto LockedDispatcher = GetSpatialOS()->GetView().Pin();

        if (LockedDispatcher.IsValid())
        {
            LockedDispatcher->Remove(entityQueryCallback);
            entityQueryCallback = -1;
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