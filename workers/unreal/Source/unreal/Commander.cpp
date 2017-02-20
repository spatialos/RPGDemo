// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
// ===========
// DO NOT EDIT - this file is automatically regenerated.
// =========== 

#include "unreal.h"
#include "Commander.h"
#include "SpatialOsComponent.h"

UCommander::UCommander()
{
}

UCommander* UCommander::Init(USpatialOsComponent* component, worker::Connection& connection, worker::View& view)
{
	mConnection = &connection;
	mView = &view;
	mComponent = component;
	mCallbacks.Reset(new improbable::unreal::callbacks::FScopedViewCallbacks(view));

	mCommandResponseDispatcherCallbacksRegistered = std::set<std::string>();
	mRequestIdToReserveEntityIdCallback = std::map<std::uint32_t, FReserveEntityIdResponse>();

	mCallbacks->Add(mView->OnReserveEntityIdResponse(std::bind(
		&UCommander::OnReserveEntityIdResponseDispatcherCallback, this, std::placeholders::_1)));

	return this;
}

void UCommander::ReserveEntityId(const FReserveEntityIdResponse& callback, int timeoutMs)
{
	if (mComponent != nullptr && !mComponent->HasAuthority())
	{
		auto rawResponse = worker::ReserveEntityIdResponseOp();
		rawResponse.StatusCode = worker::StatusCode::kFailure;
		rawResponse.Message = "Tried to send a command from a component you do not have authority on!";
		rawResponse.EntityId = worker::Option<worker::EntityId>();
		auto response = NewObject<UReserveEntityIdResponse>(this, UReserveEntityIdResponse::StaticClass())->Init(rawResponse);
		callback.ExecuteIfBound(response);
		return;
	}
	auto requestId = mConnection->SendReserveEntityIdRequest(timeoutMs);
	mRequestIdToReserveEntityIdCallback.emplace(requestId.Id, callback);
}

void UCommander::OnReserveEntityIdResponseDispatcherCallback(const worker::ReserveEntityIdResponseOp& op)
{
	if (mRequestIdToReserveEntityIdCallback.find(op.RequestId.Id) == mRequestIdToReserveEntityIdCallback.end())
	{
		return;
	}
	auto callback = mRequestIdToReserveEntityIdCallback[op.RequestId.Id];
	auto response = NewObject<UReserveEntityIdResponse>(this, UReserveEntityIdResponse::StaticClass())->Init(op);
	callback.ExecuteIfBound(response);
}