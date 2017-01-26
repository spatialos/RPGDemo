// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "TransformReceiver.h"
#include "Conversions.h"
#include "WorkerConnection.h"
#include "unrealGameMode.h"

using namespace improbable::unreal::core;

// Sets default values for this component's properties
UTransformReceiver::UTransformReceiver()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	mCallbacks = nullptr;

	mLocation = FVector();
	mRotation = FQuat();

	EntityId = AunrealGameMode::GetSpawner()->GetEntityId(GetOwner());
}

FVector UTransformReceiver::GetLocation() const
{
	return mLocation;
}

FQuat UTransformReceiver::GetRotation() const
{
	return mRotation;
}

void UTransformReceiver::OnTransformComponentUpdate(const worker::ComponentUpdateOp<improbable::corelibrary::transforms::TransformState>& op)
{
	ParseTransformStateUpdate(op);
}

void UTransformReceiver::ParseTransformStateUpdate(const worker::ComponentUpdateOp<improbable::corelibrary::transforms::TransformState>& op)
{
	const worker::Option<improbable::corelibrary::math::FixedPointVector3> loc = op.Update.local_position();
	const worker::Option<improbable::corelibrary::math::Quaternion32> rot = op.Update.local_rotation();

	if (!loc.empty())
	{
		mLocation = ToUnrealVector(*loc);
	}
	if (!rot.empty())
	{
		mRotation = ToUnrealQuaternion((*rot).quaternion());
	}
}

// Called when the game starts
void UTransformReceiver::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTransformReceiver::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	Initialise();
}

void UTransformReceiver::Initialise()
{
	if (!mCallbacks.IsValid())
	{
		worker::Option<improbable::corelibrary::transforms::TransformStateData> transform = GetEntity()->Get<improbable::corelibrary::transforms::TransformState>();
		if (!transform.empty())
		{
			mLocation = ToUnrealVector(transform->local_position());
			mRotation = ToUnrealQuaternion(transform->local_rotation().quaternion());
			//GetOwner()->SetActorLocationAndRotation(mLocation, mRotation);
			// have commented above out as setting the initial rotation from the initial transform state
			// seems to skew the rotation of the actor, probably due to the conversions not being accurate yet
			GetOwner()->SetActorLocation(mLocation);
			UE_LOG(LogTemp, Warning, TEXT("Set initial position for entity's actor"))
		}

		mCallbacks.Reset(new improbable::unreal::callbacks::FScopedViewCallbacks(FWorkerConnection::GetView()));
		mCallbacks->Add(FWorkerConnection::GetView().OnComponentUpdate<improbable::corelibrary::transforms::TransformState>(
			std::bind(&UTransformReceiver::OnTransformComponentUpdate, this, std::placeholders::_1)));
	}
}

worker::Entity* UTransformReceiver::GetEntity() const
{
	auto it = FWorkerConnection::GetView().Entities.find(EntityId);
	if (it == FWorkerConnection::GetView().Entities.end())
	{
		return nullptr;
	}
	return &(it->second);
}
