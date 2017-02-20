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
    const worker::ComponentUpdateOp<improbable::common::Transform>& op)
{
    if (EntityId != -1)
    {
        if (op.EntityId == EntityId)
        {
            ParseTransformStateUpdate(op);
        }
    }
}

void UTransformReceiver::ParseTransformStateUpdate( const worker::ComponentUpdateOp<improbable::common::Transform>& op)
{
    if (op.Update.position())
    {
        mLocation = ToUnrealPosition(*op.Update.position());
    }
    if (op.Update.rotation())
    {
        //mRotation = ToUnrealRotation(*op.Update.rotation());
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
        return;
    }

    if (!mCallbacks.IsValid())
    {
        const auto* const entity = GetEntity();

        if (entity != nullptr)
        {
           auto transform = entity->Get<improbable::common::Transform>();

            if (transform && !entity->HasAuthority<improbable::common::Transform>())
            {
                mLocation = ToUnrealPosition(transform->position());
                //mRotation = ToUnrealRotation(transform->rotation());
                //GetOwner()->SetActorLocationAndRotation(mLocation, mRotation);
                GetOwner()->SetActorLocation(mLocation);
            }

            mCallbacks.Reset(new improbable::unreal::callbacks::FScopedViewCallbacks( FWorkerConnection::GetView()));
            mCallbacks->Add(
                FWorkerConnection::GetView()
                    .OnComponentUpdate<improbable::common::Transform>(
                        std::bind(&UTransformReceiver::OnTransformComponentUpdate, this, std::placeholders::_1)));
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