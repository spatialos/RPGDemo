// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "SpatialOsComponent.h"

// Sets default values for this component's properties
USpatialOsComponent::USpatialOsComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You
    // can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

// Called when the game starts
void USpatialOsComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
}

// Called every frame
void USpatialOsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void USpatialOsComponent::Init(worker::Connection& Connection, worker::View& View,
                               worker::EntityId EntityId)
{
    mConnection = &Connection;
    mView = &View;
    mEntityId = EntityId;
    mCallbacks.Reset(new improbable::unreal::callbacks::FScopedViewCallbacks(View));
}

worker::EntityId USpatialOsComponent::GetEntityId()
{
    return mEntityId;
}

bool USpatialOsComponent::HasAuthority()
{
    return mHasAuthority;
}

bool USpatialOsComponent::IsComponentReady()
{
    return mIsComponentReady;
}

void USpatialOsComponent::OnAuthorityChangeDispatcherCallback(const worker::AuthorityChangeOp& op)
{
    if (op.EntityId != mEntityId)
    {
        return;
    }
    mHasAuthority = op.HasAuthority;
    OnAuthorityChange.Broadcast(op.HasAuthority);
}

void USpatialOsComponent::OnRemoveComponentDispatcherCallback(const worker::RemoveComponentOp& op)
{
    if (op.EntityId != mEntityId)
    {
        return;
    }
    mIsComponentReady = false;
}
