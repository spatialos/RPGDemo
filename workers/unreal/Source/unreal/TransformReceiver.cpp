// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "TransformReceiver.h"
#include "Conversions.h"
#include "EntityId.h"
#include "WorkerConnection.h"
#include "unrealGameMode.h"

using namespace improbable::unreal::core;

// Sets default values for this component's properties
UTransformReceiver::UTransformReceiver()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You
    // can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    mCallbacks = nullptr;

    mLocation = FVector();
    mRotation = FQuat();
}

FVector UTransformReceiver::GetLocation() const
{
    return mLocation;
}

FQuat UTransformReceiver::GetRotation() const
{
    return mRotation;
}

void UTransformReceiver::OnTransformComponentUpdate(
    const worker::ComponentUpdateOp<improbable::corelibrary::transforms::TransformState>& op)
{
    if (EntityId != -1)
    {
        if (op.EntityId == EntityId)
        {
            ParseTransformStateUpdate(op);
        }
    }
}

void UTransformReceiver::ParseTransformStateUpdate(
    const worker::ComponentUpdateOp<improbable::corelibrary::transforms::TransformState>& op)
{
    const worker::Option<improbable::corelibrary::math::FixedPointVector3> loc =
        op.Update.local_position();
    const worker::Option<improbable::corelibrary::math::Quaternion32> rot =
        op.Update.local_rotation();

    if (!loc.empty())
    {
        mLocation = ToUnrealVector(*loc);
        UE_LOG(LogTemp, Warning,
               TEXT("UTransformReceiver: Received actor (%s) position update (%s)"),
               *GetOwner()->GetName(), *mLocation.ToString())
    }
    if (!rot.empty())
    {
        mRotation = ToUnrealQuaternion((*rot).quaternion());
        UE_LOG(LogTemp, Warning,
               TEXT("UTransformReceiver: Received actor (%s) rotation update (%s)"),
               *GetOwner()->GetName(), *mRotation.ToString())
    }
}

// Called when the game starts
void UTransformReceiver::BeginPlay()
{
    Super::BeginPlay();

    EntityId = AunrealGameMode::GetSpawner()->GetEntityId(GetOwner());
    UE_LOG(LogTemp, Warning, TEXT("UTransformReceiver: Initial entity id got set to (%s)"),
           *ToString(EntityId))
}

// Called every frame
void UTransformReceiver::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    Initialise();
}

void UTransformReceiver::Initialise()
{
    if (EntityId == -1)
    {
        EntityId = AunrealGameMode::GetSpawner()->GetEntityId(GetOwner());
        UE_LOG(LogTemp, Warning, TEXT("UTransformReceiver: Entity id not set to a valid entity id, "
                                      "getting new entity id %s"),
               *ToString(EntityId))
        return;
    }

    if (!mCallbacks.IsValid())
    {
        const auto* const entity = GetEntity();

        if (entity != nullptr)
        {
            worker::Option<improbable::corelibrary::transforms::TransformStateData> transform =
                entity->Get<improbable::corelibrary::transforms::TransformState>();

            if (!transform.empty() &&
                !entity->HasAuthority<improbable::corelibrary::transforms::TransformState>())
            {
                mLocation = ToUnrealVector(transform->local_position());
                mRotation = ToUnrealQuaternion(transform->local_rotation().quaternion());
                // GetOwner()->SetActorLocationAndRotation(mLocation, mRotation);
                // have commented above out as setting the initial rotation from the initial
                // transform state
                // seems to skew the rotation of the actor, probably due to the conversions not
                // being
                // accurate
                // yet
                GetOwner()->SetActorLocation(mLocation);
                UE_LOG(LogTemp, Warning, TEXT("UTransformReceiver: Set initial position for actor "
                                              "(%s), position, (%s) rotation (%s)"),
                       *GetOwner()->GetName(), *mLocation.ToString(), *mRotation.ToString())
            }

            mCallbacks.Reset(new improbable::unreal::callbacks::FScopedViewCallbacks(
                FWorkerConnection::GetView()));
            mCallbacks->Add(
                FWorkerConnection::GetView()
                    .OnComponentUpdate<improbable::corelibrary::transforms::TransformState>(
                        std::bind(&UTransformReceiver::OnTransformComponentUpdate, this,
                                  std::placeholders::_1)));
        }
    }
}

worker::Entity* UTransformReceiver::GetEntity() const
{
    auto it = FWorkerConnection::GetView().Entities.find(EntityId);
    if (it == FWorkerConnection::GetView().Entities.end())
    {
        UE_LOG(LogTemp, Warning,
               TEXT("UTransformReceiver: returned nullptr trying to get entity %s on Actor %s"),
               *ToString(EntityId), *GetOwner()->GetName())
        return nullptr;
    }
	
    return &(it->second);
}
