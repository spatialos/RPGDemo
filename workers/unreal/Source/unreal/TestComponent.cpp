// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "TestComponent.h"

// Sets default values for this component's properties
UTestComponent::UTestComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You
    // can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    mConnection = nullptr;
    mView = nullptr;
    mCallbacks = nullptr;

    mHasAuthority = false;
    mIsComponentReady = false;
}

// Called when the game starts
void UTestComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
}

// Called every frame
void UTestComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                   FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

int UTestComponent::GetComponentId()
{
    return mComponentId;
}

void UTestComponent::Init(worker::Connection& Connection, worker::View& View,
                          worker::EntityId EntityId)
{
    USpatialOsComponent::Init(Connection, View, EntityId);

    mCallbacks->Add(mView->OnAuthorityChange<improbable::test::TestState>(std::bind(
        &UTestComponent::OnAuthorityChangeDispatcherCallback, this, std::placeholders::_1)));
    mCallbacks->Add(mView->OnAddComponent<improbable::test::TestState>(
        std::bind(&UTestComponent::OnAddComponentDispatcherCallback, this, std::placeholders::_1)));
    mCallbacks->Add(mView->OnRemoveComponent<improbable::test::TestState>(std::bind(
        &UTestComponent::OnRemoveComponentDispatcherCallback, this, std::placeholders::_1)));
    mCallbacks->Add(mView->OnComponentUpdate<improbable::test::TestState>(std::bind(
        &UTestComponent::OnComponentUpdateDispatcherCallback, this, std::placeholders::_1)));

    mCallbacks->Add(mView->OnCommandRequest<improbable::test::TestState::Commands::Damage>(
        std::bind(&UTestComponent::OnDamageCommandRequestDispatcherCallback, this,
                  std::placeholders::_1)));
}

int UTestComponent::GetInt32Val()
{
    return mInt32Val;
}

void UTestComponent::SendComponentUpdate(UTestStateUpdate* update)
{
    mConnection->SendComponentUpdate<improbable::test::TestState>(mEntityId, update->GetRawUpdate());
}

void UTestComponent::OnAddComponentDispatcherCallback(
    const worker::AddComponentOp<improbable::test::TestState>& op)
{
    if (op.EntityId != mEntityId)
    {
        return;
    }
    auto update = improbable::test::TestState::Update::FromInitialData(op.Data);
    ApplyComponentUpdate(update);
}

void UTestComponent::OnComponentUpdateDispatcherCallback(
    const worker::ComponentUpdateOp<improbable::test::TestState>& op)
{
    if (op.EntityId != mEntityId)
    {
        return;
    }
    auto update = op.Update;
    ApplyComponentUpdate(update);
}

void UTestComponent::ApplyComponentUpdate(const improbable::test::TestState::Update& update)
{
    if (!update.int32_val().empty())
    {
        mInt32Val = *update.int32_val().data();
        OnInt32ValUpdate.Broadcast(mInt32Val);
    }

    if (!update.text_event().empty())
    {
        for (auto& val : update.text_event())
        {
			auto stringWrapper = NewObject<UStringWrapper>(this, UStringWrapper::StaticClass());
			stringWrapper->Init(val);
            OnTextEvent.Broadcast(stringWrapper);
        }
    }

	auto testStateUpdate = NewObject<UTestStateUpdate>(this, UTestStateUpdate::StaticClass());
	testStateUpdate->Init(update);
    OnComponentUpdate.Broadcast(testStateUpdate);

    if (!mIsComponentReady)
    {
        mIsComponentReady = true;
        OnComponentReady.Broadcast();
    }
}

void UTestComponent::OnDamageCommandRequestDispatcherCallback(
    const worker::CommandRequestOp<improbable::test::TestState::Commands::Damage>& op)
{
    if (op.EntityId != mEntityId)
    {
        return;
    }
	auto request = NewObject<UDamageRequest>(this, UDamageRequest::StaticClass());
	request->Init(op.Request);
	auto responder = NewObject<UDamageCommandResponder>(this, UDamageCommandResponder::StaticClass());
	responder->Init(mConnection, op.RequestId, request);
    OnDamageCommandRequest.Broadcast(responder);
}