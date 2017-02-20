// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "TransformSender.h"
#include "Conversions.h"
#include "EntityId.h"
#include "WorkerConnection.h"
#include "improbable/common//transform.h"
#include "unrealGameMode.h"

using namespace improbable::unreal::core;

// Sets default values for this component's properties
UTransformSender::UTransformSender()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You
    // can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTransformSender::BeginPlay()
{
    Super::BeginPlay();
    EntityId = AunrealGameMode::GetSpawner()->GetEntityId(GetOwner());
}

// Called every frame
void UTransformSender::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (EntityId == -1)
    {
        EntityId = AunrealGameMode::GetSpawner()->GetEntityId(GetOwner());

        if (EntityId != -1)
        {
            const auto* const entity = GetEntity();

            if (entity != nullptr)
            {
                auto transform = entity->Get<improbable::common::Transform>();

                if (!transform.empty() && HasAuthority())
                {
                    FVector location = ToUnrealPosition(transform->position());
                    FQuat rotation = ToUnrealRotation(transform->rotation());

                    auto* const owner = GetOwner();
                    owner->SetActorLocation(location);
                    //owner->SetActorRotation(rotation);
                }
            }
        }

        return;
    }

    if (!HasAuthority())
	{
        return;
    }

    FVector location = GetOwner()->GetActorLocation();
    improbable::math::Coordinates locationUpdate = ToNativeCoordinates(location);

    FQuat rotation = GetOwner()->GetActorRotation().Quaternion();
    worker::List<float> rotationUpdate = ToNativeRotation(rotation);

    auto entity = GetEntity();
    if (entity != nullptr)
    {
        auto previous = entity->Get<improbable::common::Transform>();
        if (previous.empty()
            || previous->position() != locationUpdate
            || previous->rotation() != rotationUpdate)
        {
            improbable::common::Transform::Update update;
            update.set_position(locationUpdate)
                .set_rotation(rotationUpdate);

            entity->Update<improbable::common::Transform>(update);
            FWorkerConnection::GetConnection().SendComponentUpdate<improbable::common::Transform>(EntityId, update);
        }
    }
}

bool UTransformSender::HasAuthority() const
{
    if (GetEntity() == nullptr)
    {
        return false;
    }

    return GetEntity()->HasAuthority<improbable::common::Transform>();
}

worker::Entity* UTransformSender::GetEntity() const
{
    auto it = FWorkerConnection::GetView().Entities.find(EntityId);
    if (it == FWorkerConnection::GetView().Entities.end())
    {
        return nullptr;
    }
    return &(it->second);
}
