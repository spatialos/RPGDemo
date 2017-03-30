// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "RPGDemoGameInstance.h"
#include "ConversionsFunctionLibrary.h"
#include "RpgDemoGameMode.h"
#include "RpgDemoPlayerController.h"
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

FString ARpgDemoGameMode::GetSpatialOsWorkerType()
{
    return GetSpatialOS()->GetWorkerType();
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
    mGetSpawnerEntityIdResultCallback = new FGetSpawnerEntityIdResultDelegate(callback);
    const worker::query::EntityQuery& entity_query = {
        worker::query::ComponentConstraint{spawner::Spawner::ComponentId},
        worker::query::SnapshotResultType{}};
    auto requestId =
		GetSpatialOS()->GetWorkerConnection().GetConnection().SendEntityQueryRequest(
            entity_query, static_cast<std::uint32_t>(timeoutMs));
    entityQueryCallback =
		GetSpatialOS()->GetWorkerConnection().GetView().OnEntityQueryResponse(
            [this, requestId](const worker::EntityQueryResponseOp& op) {
                if (op.RequestId != requestId)
                {
                    return;
                }
                if (!mGetSpawnerEntityIdResultCallback->IsBound())
                {
                    UE_LOG(LogTemp, Warning, TEXT("mGetSpawnerEntityIdResultCallback is unbound"))
                }
                if (op.StatusCode != worker::StatusCode::kSuccess)
                {
                    std::string errorMessage = "Could not find spawner entity: " + op.Message;
                    mGetSpawnerEntityIdResultCallback->ExecuteIfBound(
                        false, FString(errorMessage.c_str()), -1);
                    return;
                }
                if (op.ResultCount == 0)
                {
                    std::string errorMessage = "Query returned 0 spawner entities";
                    mGetSpawnerEntityIdResultCallback->ExecuteIfBound(
                        false, FString(errorMessage.c_str()), -1);
                    return;
                }
                mGetSpawnerEntityIdResultCallback->ExecuteIfBound(
                    true, FString(), static_cast<int>(op.Result.begin()->first));
                GetWorldTimerManager().SetTimerForNextTick(UnbindEntityQueryDelegate);
                return;
            });
}

void ARpgDemoGameMode::StartPlay()
{
    AGameModeBase::StartPlay();

	GetSpatialOS()->RegisterBlueprintFolder(TEXT(ENTITY_BLUEPRINTS_FOLDER));
	GetSpatialOS()->OnConnectedDelegate.BindUObject(
        this, &ARpgDemoGameMode::OnSpatialOsConnected);
	GetSpatialOS()->OnConnectionFailedDelegate.BindUObject(
        this, &ARpgDemoGameMode::OnSpatialOsFailedToConnect);
	GetSpatialOS()->OnDisconnectedDelegate.BindUObject(
        this, &ARpgDemoGameMode::OnSpatialOsDisconnected);
    UE_LOG(LogSpatialOS, Display, TEXT("Startplay called to SpatialOS"))
	GetSpatialOS()->CreateWorkerConnection(GetWorld(), WorkerTypeOverride,
                                                      WorkerIdOverride);
}

void ARpgDemoGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AGameModeBase::EndPlay(EndPlayReason);
	GetSpatialOS()->TeardownWorkerConnection();
}

void ARpgDemoGameMode::Tick(float DeltaTime)
{
    AGameModeBase::Tick(DeltaTime);
	GetSpatialOS()->ProcessEvents();
}

bool ARpgDemoGameMode::IsConnectedToSpatialOs()
{
    return GetSpatialOS()->IsConnectedToSpatialOs();
}

UCommander* ARpgDemoGameMode::SendWorkerCommand()
{
    if (Commander == nullptr)
    {
        auto& WorkerConnection = GetSpatialOS()->GetWorkerConnection();
        Commander =
            NewObject<UCommander>(this, UCommander::StaticClass())
                ->Init(nullptr, &WorkerConnection.GetConnection(), &WorkerConnection.GetView());
    }
    return Commander;
}

void ARpgDemoGameMode::UnbindEntityQueryCallback()
{
    if (entityQueryCallback != -1)
    {
		GetSpatialOS()->GetWorkerConnection().GetView().Remove(entityQueryCallback);
        entityQueryCallback = -1;
    }
}

FSpatialOS* ARpgDemoGameMode::GetSpatialOS()
{
	auto gameInstance = Cast<URPGDemoGameInstance>(GetWorld()->GetGameInstance());

	if(gameInstance != nullptr)
	{
		return &gameInstance->GetSpatialOS();
	}

	return nullptr;
}