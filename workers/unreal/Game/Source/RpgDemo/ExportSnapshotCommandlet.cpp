// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"

#include "ExportSnapshotCommandlet.h"

#define IMPROBABLE_MATH_NO_PROTO 1
//#include "improbable/standard_library.h"
//#include "improbable/math/vector3d.h"
#include <improbable/worker.h>
#undef IMPROBABLE_MATH_NO_PROTO
#include "improbable/collections.h"
#include "improbable/standard_library.h"
#include <improbable/spawner/spawner.h>
#include <array>

using namespace improbable;

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
    worker::Option<std::string> Result =
        worker::SaveSnapshot(TCHAR_TO_UTF8(*fullPath), snapshotEntities);
    if (!Result.empty())
    {
        std::string ErrorString = Result.value_or("");
        UE_LOG(LogTemp, Display, TEXT("Error: %s"), UTF8_TO_TCHAR(ErrorString.c_str()));
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("Snapshot exported to the path %s"), *fullPath);
    }
}

const std::array<float, 5> verticalCorridors = {{-2.0f, 2.75f, 7.5f, 12.25f, 17.0f}};
const std::array<float, 5> horizontalCorridors = {{-10.0f, -4.0f, 2.0f, 8.0f, 14.0f}};

worker::SnapshotEntity UExportSnapshotCommandlet::CreateNPCSnapshotEntity() const
{
    const int randomVerticalCorridor = FMath::RandRange(0, verticalCorridors.size());
    const int randomHorizontalCorridor = FMath::RandRange(0, horizontalCorridors.size());

    const Coordinates initialPosition{verticalCorridors[randomVerticalCorridor], 4.0,
                                      horizontalCorridors[randomHorizontalCorridor]};
    // const worker::List<float> initialRotation{1.0f, 0.0f, 0.0f, 0.0f};

    auto snapshotEntity = worker::SnapshotEntity();
    snapshotEntity.Prefab = "Npc";
    snapshotEntity.Add<Position>(Position::Data{initialPosition});
    snapshotEntity.Add<Metadata>(Metadata::Data{"Npc"});
    snapshotEntity.Add<Persistence>(Persistence::Data{});

    WorkerAttributeSet unrealWorkerAttributeSet{worker::List<std::string>{"UnrealWorker"}};
    WorkerAttributeSet unrealClientAttributeSet{worker::List<std::string>{"UnrealClient"}};

    WorkerRequirementSet unrealWorkerWritePermission{{unrealWorkerAttributeSet}};
    WorkerRequirementSet unrealClientWritePermission{{unrealClientAttributeSet}};
    WorkerRequirementSet anyWorkerReadPermission{
        {unrealClientAttributeSet, unrealWorkerAttributeSet}};

    worker::Map<std::uint32_t, WorkerRequirementSet> componentAuthority;
    componentAuthority.emplace(Position::ComponentId, unrealWorkerWritePermission);
    componentAuthority.emplace(Metadata::ComponentId, unrealWorkerWritePermission);
    componentAuthority.emplace(Persistence::ComponentId, unrealWorkerWritePermission);

    snapshotEntity.Add<EntityAcl>(EntityAcl::Data(anyWorkerReadPermission, componentAuthority));

    return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateSpawnerSnapshotEntity() const
{
    const Coordinates initialPosition{0, 0, 0};
    const worker::List<float> initialRotation{1.0f, 0.0f, 0.0f, 0.0f};

    auto snapshotEntity = worker::SnapshotEntity();
    snapshotEntity.Prefab = "Spawner";

    snapshotEntity.Add<Metadata>(Metadata::Data("Spawner"));
    snapshotEntity.Add<Position>(Position::Data{initialPosition});
    snapshotEntity.Add<spawner::Spawner>(spawner::Spawner::Data{});
    snapshotEntity.Add<Persistence>(Persistence::Data{});

    WorkerAttributeSet unrealWorkerAttributeSet{worker::List<std::string>{"UnrealWorker"}};
    WorkerAttributeSet unrealClientAttributeSet{worker::List<std::string>{"UnrealClient"}};

    WorkerRequirementSet unrealWorkerWritePermission{{unrealWorkerAttributeSet}};
    WorkerRequirementSet anyWorkerReadPermission{
        {unrealClientAttributeSet, unrealWorkerAttributeSet}};

    worker::Map<std::uint32_t, WorkerRequirementSet> componentAuthority;
    componentAuthority.emplace(Position::ComponentId, unrealWorkerWritePermission);
    componentAuthority.emplace(spawner::Spawner::ComponentId, unrealWorkerWritePermission);
    componentAuthority.emplace(Metadata::ComponentId, unrealWorkerWritePermission);
    componentAuthority.emplace(Persistence::ComponentId, unrealWorkerWritePermission);

    snapshotEntity.Add<EntityAcl>(EntityAcl::Data(anyWorkerReadPermission, componentAuthority));

    return snapshotEntity;
}