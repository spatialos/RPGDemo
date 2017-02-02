// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"

#include "ExportSnapshotCommandlet.h"

#include "Conversions.h"
#include "improbable/collections.h"
#include "improbable/corelib/entity/prefab.h"
#include "improbable/corelib/physical/physicality.h"
#include "improbable/corelib/visual/visuality.h"
#include "improbable/corelibrary/transforms/global/global_transform_publisher_state.h"
#include "improbable/corelibrary/transforms/global/global_transform_state.h"
#include "improbable/corelibrary/transforms/teleport/teleport_ack_state.h"
#include "improbable/corelibrary/transforms/teleport/teleport_request_state.h"
#include "improbable/corelibrary/transforms/transform_exception_state.h"
#include "improbable/corelibrary/transforms/transform_hierarchy_state.h"
#include "improbable/corelibrary/transforms/transform_state.h"
#include "improbable/entity/physical/tags_data.h"
#include "improbable/math/coordinates.h"
#include "improbable/math/vector3d.h"
#include "improbable/math/vector3f.h"

#include <improbable/worker.h>

#include "improbable/standard_library.h"

using namespace improbable;
using namespace improbable::corelib::physical;
using namespace improbable::corelib::visual;
using namespace improbable::corelib::entity;
using namespace improbable::entity::physical;
using namespace improbable::corelibrary::transforms;
using namespace improbable::corelibrary::math;
using namespace improbable::math;
using namespace improbable::corelibrary::transforms::global;
using namespace improbable::corelibrary::transforms::teleport;
using namespace improbable::corelib::math;
using namespace improbable::corelibrary::subscriptions;

UExportSnapshotCommandlet::UExportSnapshotCommandlet() {}

UExportSnapshotCommandlet::~UExportSnapshotCommandlet() {}

int32 UExportSnapshotCommandlet::Main(const FString& Params) {
  FString combinedPath = FPaths::Combine(*FPaths::GetPath(FPaths::GetProjectFilePath()), TEXT("../../snapshots"));
  UE_LOG(LogTemp, Display, TEXT("Combined path %s"), *combinedPath);
  if (FPaths::CollapseRelativeDirectories(combinedPath)) {
    FString fullPath = FPaths::Combine(*combinedPath, TEXT("default.snapshot"));

    worker::SaveSnapshot(TCHAR_TO_UTF8(*fullPath), {{454, CreateNPCSnapshotEntity()}});
    UE_LOG(LogTemp, Display, TEXT("Snapshot exported to the path %s"), *fullPath);
  } else {
    UE_LOG(LogTemp, Display, TEXT("bye world!"));
  }

  return 0;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateNPCSnapshotEntity() const {
  auto snapshotEntity = worker::SnapshotEntity();
  snapshotEntity.Add<Physicality>(Physicality::Data(true));
  snapshotEntity.Add<Visuality>(Visuality::Data(true));
  snapshotEntity.Add<Prefab>(Prefab::Data("Npc"));
  snapshotEntity.Add<TagsData>(TagsData::Data(worker::List<std::string>()));
  snapshotEntity.Add<TransformState>(
      TransformState::Data(FixedPointVector3(worker::List<std::int64_t>({0, 0, 0})),
                           Quaternion32(ToQuaternion32(0, 0, 0, 1)), Parent(-1, ""),
                           Vector3d(0, 0, 0), Vector3f(0, 0, 0), Vector3f(0, 0, 0), false, 0.0f));
  snapshotEntity.Add<GlobalTransformState>(GlobalTransformState::Data(
      Coordinates(0, 0, 0), Quaternion(0, 0, 0, 1), Vector3d(0, 0, 0), 0.0f));
  snapshotEntity.Add<TransformExceptionState>(
      TransformExceptionState::Data(worker::Option<worker::EntityId>()));
  snapshotEntity.Add<GlobalTransformPublisherState>(GlobalTransformPublisherState::Data(
      SubscriberData(worker::Map<std::string, SubscribedEntities>(), 0)));
  snapshotEntity.Add<TransformHierarchyState>(TransformHierarchyState::Data(worker::List<Child>()));
  snapshotEntity.Add<TeleportRequestState>(TeleportRequestState::Data(
      Vector3d(0, 0, 0), worker::Option<Quaternion>(), worker::Option<Parent>(), 0));
  snapshotEntity.Add<TeleportAckState>(TeleportAckState::Data(0));

  improbable::WorkerPredicate fsimPredicate({{{{{"UnrealWorker"}}}}});

  worker::Map<std::uint32_t, improbable::WorkerPredicate> componentAuthority;

  componentAuthority.emplace(Prefab::ComponentId, fsimPredicate);
  componentAuthority.emplace(TransformState::ComponentId, fsimPredicate);

  improbable::ComponentAcl componentAcl(componentAuthority);

  snapshotEntity.Add<EntityAcl>(EntityAcl::Data(fsimPredicate, componentAcl));

  return snapshotEntity;
}