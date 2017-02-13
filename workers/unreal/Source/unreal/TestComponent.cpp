// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "TestComponent.h"

// Sets default values for this component's properties
UTestComponent::UTestComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
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
void UTestComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UTestComponent::Init(worker::Connection& Connection, worker::View& View, worker::EntityId EntityId)
{
	mConnection.Reset(&Connection);
	mView.Reset(&View);
	mEntityId = EntityId;
	mCallbacks.Reset(new improbable::unreal::callbacks::FScopedViewCallbacks(View));

	mCallbacks->Add(mView->OnAuthorityChange<improbable::test::TestState>(
		std::bind(&UTestComponent::OnAuthorityChangeDispatcherCallback, this, std::placeholders::_1)
	));
	mCallbacks->Add(mView->OnAddComponent<improbable::test::TestState>(
		std::bind(&UTestComponent::OnAddComponentDispatcherCallback, this, std::placeholders::_1)
	));
	mCallbacks->Add(mView->OnRemoveComponent<improbable::test::TestState>(
		std::bind(&UTestComponent::OnRemoveComponentDispatcherCallback, this, std::placeholders::_1)
	));
	mCallbacks->Add(mView->OnComponentUpdate<improbable::test::TestState>(
		std::bind(&UTestComponent::OnComponentUpdateDispatcherCallback, this, std::placeholders::_1)
	));
}

bool UTestComponent::HasAuthority()
{
	return mHasAuthority;
}

bool UTestComponent::IsComponentReady()
{
	return mIsComponentReady;
}

void UTestComponent::OnAuthorityChangeDispatcherCallback(const worker::AuthorityChangeOp& op)
{
	if (op.EntityId != mEntityId)
	{
		return;
	}
	mHasAuthority = op.HasAuthority;
	OnAuthorityChange.Broadcast(op.HasAuthority);
}

void UTestComponent::OnAddComponentDispatcherCallback(const worker::AddComponentOp<improbable::test::TestState> op)
{
	if (op.EntityId != mEntityId)
	{
		return;
	}
	auto update = improbable::test::TestState::Update::FromInitialData(op.Data);
	ApplyComponentUpdate(update);
}

void UTestComponent::OnRemoveComponentDispatcherCallback(const worker::RemoveComponentOp op)
{
	if (op.EntityId != mEntityId)
	{
		return;
	}
	mIsComponentReady = false;
}

void UTestComponent::OnComponentUpdateDispatcherCallback(const worker::ComponentUpdateOp<improbable::test::TestState> op)
{
	if (op.EntityId != mEntityId)
	{
		return;
	}
	auto update = op.Update;
	ApplyComponentUpdate(update);
}

void UTestComponent::ApplyComponentUpdate(const improbable::test::TestState::Update update)
{
	if (!update.int32_val().empty())
	{
		mInt32Val = *update.int32_val().data();
		OnInt32ValUpdate.Broadcast(mInt32Val);
	}

	if (!update.text_event().empty())
	{
		for (auto &val : update.text_event())
		{
			OnTextEvent.Broadcast(val);
		}
	}

	OnComponentUpdate.Broadcast(update);

	if (!mIsComponentReady)
	{
		mIsComponentReady = true;
		OnComponentReady.Broadcast();
	}
}