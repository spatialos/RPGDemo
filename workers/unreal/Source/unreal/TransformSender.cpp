// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "TransformSender.h"
#include "Conversions.h"
#include "WorkerConnection.h"
#include "improbable/corelibrary/transforms/transform_state.h"
#include "unrealGameMode.h"

using namespace improbable::unreal::core;

// Sets default values for this component's properties
UTransformSender::UTransformSender() {
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You
  // can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTransformSender::BeginPlay() {
  Super::BeginPlay();
  EntityId = AunrealGameMode::GetSpawner()->GetEntityId(GetOwner());
}

// Called every frame
void UTransformSender::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (!HasAuthority())
    return;

  FVector location = GetOwner()->GetActorLocation();
  improbable::corelibrary::math::FixedPointVector3 locationUpdate =
      ToFixedPoint(UnrealToSpatialPosition(location));

  FQuat rotation = GetOwner()->GetActorRotation().Quaternion();
  uint32_t rotationUpdate = ToQuaternion32(UnrealToSpatialQuaternion(rotation));

  auto previous = GetEntity()->Get<improbable::corelibrary::transforms::TransformState>();
  if (previous.empty() || previous->local_position() != locationUpdate ||
      previous->local_rotation() != rotationUpdate) {
    improbable::corelibrary::transforms::TransformState::Update update;
    update.set_local_position(locationUpdate);
    update.set_local_rotation(rotationUpdate);

    UE_LOG(LogTemp, Warning, TEXT("Sending transform state update"))
    GetEntity()->Update<improbable::corelibrary::transforms::TransformState>(update);
    FWorkerConnection::GetConnection()
        .SendComponentUpdate<improbable::corelibrary::transforms::TransformState>(EntityId, update);
  }
}

bool UTransformSender::HasAuthority() const {
  return GetEntity()->HasAuthority<improbable::corelibrary::transforms::TransformState>();
}

worker::Entity* UTransformSender::GetEntity() const {
  auto it = FWorkerConnection::GetView().Entities.find(EntityId);
  if (it == FWorkerConnection::GetView().Entities.end()) {
    return nullptr;
  }
  return &(it->second);
}
