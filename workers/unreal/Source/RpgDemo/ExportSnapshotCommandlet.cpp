// Fill out your copyright notice in the Description page of Project Settings.

#include "RpgDemo.h"

#include "ExportSnapshotCommandlet.h"
#include "improbable/collections.h"
#include "improbable/math/coordinates.h"
#include "improbable/math/vector3d.h"
#include <improbable/common/transform.h>

#include <improbable/worker.h>

#include "improbable/standard_library.h"
#include "improbable/test/test.h"

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
        FPaths::Combine(*FPaths::GetPath(FPaths::GetProjectFilePath()), TEXT("../../snapshots"));
    UE_LOG(LogTemp, Display, TEXT("Combined path %s"), *combinedPath);
    if (FPaths::CollapseRelativeDirectories(combinedPath))
    {
        FString fullPath = FPaths::Combine(*combinedPath, TEXT("default.snapshot"));

        worker::SaveSnapshot(TCHAR_TO_UTF8(*fullPath), {{454, CreateNPCSnapshotEntity()}});
        UE_LOG(LogTemp, Display, TEXT("Snapshot exported to the path %s"), *fullPath);
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("bye world!"));
    }

    return 0;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateNPCSnapshotEntity() const
{
    const Coordinates initialPosition{ 0.0, 4.0, 0.0 };
    const worker::List<float> initialRotation{ 1.0f, 0.0f, 0.0f, 0.0f };
    auto snapshotEntity = worker::SnapshotEntity();
    snapshotEntity.Prefab = "Npc";

    snapshotEntity.Add<common::Transform>(common::Transform::Data{ initialPosition, initialRotation });
	snapshotEntity.Add<test::TestState>(test::TestState::Data{ 10, "hello world" });

    WorkerAttributeSet unrealWorkerAttributeSet{ {worker::Option<std::string>{"UnrealWorker"}} };
    WorkerAttributeSet unrealClientAttributeSet{ {worker::Option<std::string>{"UnrealClient"}} };

    WorkerRequirementSet workerRequirementSet{{unrealWorkerAttributeSet}};
	WorkerRequirementSet clientRequirementSet{ { unrealClientAttributeSet } };
    WorkerRequirementSet globalRequirmentSet{{unrealClientAttributeSet, unrealWorkerAttributeSet}};

    worker::Map<std::uint32_t, WorkerRequirementSet> componentAuthority;

    componentAuthority.emplace(common::Transform::ComponentId, workerRequirementSet);
	componentAuthority.emplace(test::TestState::ComponentId, clientRequirementSet);

    ComponentAcl componentAcl(componentAuthority);

    snapshotEntity.Add<EntityAcl>(EntityAcl::Data(globalRequirmentSet, componentAcl));

    return snapshotEntity;
}
