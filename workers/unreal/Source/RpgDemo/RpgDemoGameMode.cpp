// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "ConversionsFunctionLibrary.h"
#include "RpgDemoGameMode.h"
#include "RpgDemoPlayerController.h"
#include "WorkerConnection.h"
#include "improbable/standard_library.h"
#include <improbable/common/transform.h>
#include <improbable/player/heartbeat.h>
#include <improbable/spawner/spawner.h>

//ARpgDemoGameMode* ARpgDemoGameMode::Instance;

ARpgDemoGameMode::ARpgDemoGameMode() : entityQueryCallback(-1)
{
    // Set the default player controller class
    PlayerControllerClass = ARpgDemoPlayerController::StaticClass();

    // Don't spawn players automatically
    bStartPlayersAsSpectators = true;

    // No need for default pawn class
    DefaultPawnClass = nullptr;

 //   Instance = this;

    UnbindEntityQueryDelegate.BindUObject(this, &ARpgDemoGameMode::UnbindEntityQueryCallback);
}

ARpgDemoGameMode::~ARpgDemoGameMode()
{
 //   Instance = nullptr;
    UnbindEntityQueryCallback();
}

void ARpgDemoGameMode::Tick(float DeltaTime)
{
    ASpatialOSGameMode::Tick(DeltaTime);
}

UEntityTemplate* ARpgDemoGameMode::CreatePlayerEntityTemplate(FString clientWorkerId,
                                                              const FVector& position)
{
    const auto& spatialOsPosition =
        UConversionsFunctionLibrary::UnrealCoordinatesToSpatialOsCoordinates(position);
    const improbable::math::Coordinates initialPosition{spatialOsPosition.X, spatialOsPosition.Y,
                                                        spatialOsPosition.Z};
    const worker::List<float> initialRoation{1.0f, 0.0f, 0.0f, 0.0f};

    const improbable::WorkerAttributeSet unrealWorkerAttributeSet{
        {worker::Option<std::string>{"UnrealWorker"}}};
    const std::string clientWorkerIdString = TCHAR_TO_UTF8(*clientWorkerId);
    const std::string clientAttribute = "workerId:" + clientWorkerIdString;
    UE_LOG(LogTemp, Warning, TEXT("Making ourselves authoritative over Player Transform and "
                                  "HeartbeatReceiver with worker ID %s"),
           *FString(clientAttribute.c_str()))
    const improbable::WorkerAttributeSet ownUnrealClientAttributeSet{
        {worker::Option<std::string>{clientAttribute}}};
    const improbable::WorkerAttributeSet allUnrealClientsAttributeSet{
        {worker::Option<std::string>{"UnrealClient"}}};

    const improbable::WorkerRequirementSet workerRequirementSet{{unrealWorkerAttributeSet}};
    const improbable::WorkerRequirementSet ownClientRequirementSet{{ownUnrealClientAttributeSet}};
    const improbable::WorkerRequirementSet globalRequirementSet{
        {allUnrealClientsAttributeSet, unrealWorkerAttributeSet}};

    worker::Map<std::uint32_t, improbable::WorkerRequirementSet> componentAuthority;

    componentAuthority.emplace(improbable::common::Transform::ComponentId, ownClientRequirementSet);
    componentAuthority.emplace(improbable::player::HeartbeatReceiver::ComponentId,
                               ownClientRequirementSet);
    componentAuthority.emplace(improbable::player::HeartbeatSender::ComponentId,
                               workerRequirementSet);

    const improbable::ComponentAcl componentAcl(componentAuthority);

    worker::Entity playerTemplate;
    playerTemplate.Add<improbable::common::Transform>(
        improbable::common::Transform::Data{initialPosition, initialRoation});
    playerTemplate.Add<improbable::player::HeartbeatSender>(
        improbable::player::HeartbeatSender::Data{});
    playerTemplate.Add<improbable::player::HeartbeatReceiver>(
        improbable::player::HeartbeatReceiver::Data{});
    playerTemplate.Add<improbable::EntityAcl>(
        improbable::EntityAcl::Data{globalRequirementSet, componentAcl});
    return NewObject<UEntityTemplate>(this, UEntityTemplate::StaticClass())->Init(playerTemplate);
}

void ARpgDemoGameMode::GetSpawnerEntityId(const FGetSpawnerEntityIdResultDelegate& callback,
                                          int timeoutMs)
{
    mGetSpawnerEntityIdResultCallback = new FGetSpawnerEntityIdResultDelegate(callback);
    const worker::query::EntityQuery& entity_query = {
        worker::query::ComponentConstraint{improbable::spawner::Spawner::ComponentId},
        worker::query::SnapshotResultType{}};
    auto requestId =
        improbable::unreal::core::FWorkerConnection::GetInstance()
            .GetConnection()
            .SendEntityQueryRequest(entity_query, static_cast<std::uint32_t>(timeoutMs));
    entityQueryCallback =
        improbable::unreal::core::FWorkerConnection::GetInstance().GetView().OnEntityQueryResponse(
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

void ARpgDemoGameMode::UnbindEntityQueryCallback()
{
    if (entityQueryCallback != -1)
    {
        improbable::unreal::core::FWorkerConnection::GetInstance().GetView().Remove(
            entityQueryCallback);
        entityQueryCallback = -1;
    }
}