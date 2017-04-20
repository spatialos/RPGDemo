// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"

#include "ExportSnapshotCommandlet.h"
#include "improbable/collections.h"
#include "improbable/math/coordinates.h"
#include "improbable/math/vector3d.h"
#include <improbable/common/transform.h>
#include <improbable/spawner/spawner.h>
#include <improbable/worker.h>
#include <array>

#include "improbable/standard_library.h"

using namespace improbable;
using namespace improbable::math;

UExportSnapshotCommandlet::UExportSnapshotCommandlet()
{
}

UExportSnapshotCommandlet::~UExportSnapshotCommandlet()
{
}

int32 UExportSnapshotCommandlet::Main(const FString& Params)
{
    FString combinedPath =
        FPaths::Combine(*FPaths::GetPath(FPaths::GetProjectFilePath()), TEXT("../../../snapshots"));
    UE_LOG(LogTemp, Display, TEXT("Combined path %s"), *combinedPath);
    if (FPaths::CollapseRelativeDirectories(combinedPath))
    {
        GenerateSnapshot(combinedPath);
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Path was invalid - snapshot not generated"));
    }

    return 0;
}

void UExportSnapshotCommandlet::GenerateSnapshot(const FString& savePath) const
{
    const FString fullPath = FPaths::Combine(*savePath, TEXT("default.snapshot"));

    std::unordered_map<worker::EntityId, worker::SnapshotEntity> snapshotEntities;
    snapshotEntities.emplace(0, CreateSpawnerSnapshotEntity());
    for (int npcId = 1; npcId <= 5; npcId++)
    {
        snapshotEntities.emplace(npcId, CreateNPCSnapshotEntity());
    }
    worker::SaveSnapshot(TCHAR_TO_UTF8(*fullPath), snapshotEntities);
    UE_LOG(LogTemp, Display, TEXT("Snapshot exported to the path %s"), *fullPath);
}

const std::array<float, 5> verticalCorridors = {{-2.0f, 2.75f, 7.5f, 12.25f, 17.0f}};
const std::array<float, 5> horizontalCorridors = {{-10.0f, -4.0f, 2.0f, 8.0f, 14.0f}};

worker::SnapshotEntity UExportSnapshotCommandlet::CreateNPCSnapshotEntity() const
{
    const int randomVerticalCorridor = FMath::RandRange(0, verticalCorridors.size());
    const int randomHorizontalCorridor = FMath::RandRange(0, horizontalCorridors.size());

    const Coordinates initialPosition{verticalCorridors[randomVerticalCorridor], 4.0,
                                      horizontalCorridors[randomHorizontalCorridor]};
    const worker::List<float> initialRotation{1.0f, 0.0f, 0.0f, 0.0f};

    auto snapshotEntity = worker::SnapshotEntity();
    snapshotEntity.Prefab = "Npc";

    snapshotEntity.Add<common::Transform>(
        common::Transform::Data{initialPosition, initialRotation});

    WorkerAttributeSet unrealWorkerAttributeSet{{worker::Option<std::string>{"UnrealWorker"}}};
    WorkerAttributeSet unrealClientAttributeSet{{worker::Option<std::string>{"UnrealClient"}}};

    WorkerRequirementSet unrealWorkerWritePermission{{unrealWorkerAttributeSet}};
    WorkerRequirementSet unrealClientWritePermission{{unrealClientAttributeSet}};
    WorkerRequirementSet anyWorkerReadPermission{
        {unrealClientAttributeSet, unrealWorkerAttributeSet}};

    worker::Map<std::uint32_t, WorkerRequirementSet> componentAuthority;
    componentAuthority.emplace(common::Transform::ComponentId, unrealWorkerWritePermission);

    ComponentAcl componentWritePermissions(componentAuthority);
    snapshotEntity.Add<EntityAcl>(
        EntityAcl::Data(anyWorkerReadPermission, componentWritePermissions));

    return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateSpawnerSnapshotEntity() const
{
    const Coordinates initialPosition{0, 0, 0};
    const worker::List<float> initialRotation{1.0f, 0.0f, 0.0f, 0.0f};

    auto snapshotEntity = worker::SnapshotEntity();
    snapshotEntity.Prefab = "Spawner";

    snapshotEntity.Add<common::Transform>(
        common::Transform::Data{initialPosition, initialRotation});
    snapshotEntity.Add<spawner::Spawner>(spawner::Spawner::Data{});

    WorkerAttributeSet unrealWorkerAttributeSet{{worker::Option<std::string>{"UnrealWorker"}}};
    WorkerAttributeSet unrealClientAttributeSet{{worker::Option<std::string>{"UnrealClient"}}};

    WorkerRequirementSet unrealWorkerWritePermission{{unrealWorkerAttributeSet}};
    WorkerRequirementSet anyWorkerReadPermission{
        {unrealClientAttributeSet, unrealWorkerAttributeSet}};

    worker::Map<std::uint32_t, WorkerRequirementSet> componentAuthority;
    componentAuthority.emplace(common::Transform::ComponentId, unrealWorkerWritePermission);
    componentAuthority.emplace(spawner::Spawner::ComponentId, unrealWorkerWritePermission);

    ComponentAcl componentWritePermissions(componentAuthority);
    snapshotEntity.Add<EntityAcl>(
        EntityAcl::Data(anyWorkerReadPermission, componentWritePermissions));

    return snapshotEntity;
}