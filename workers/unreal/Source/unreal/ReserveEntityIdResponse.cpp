// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
// ===========
// DO NOT EDIT - this file is automatically regenerated.
// =========== 

#include "unreal.h"
#include "ReserveEntityIdResponse.h"

UReserveEntityIdResponse::UReserveEntityIdResponse()
{
	mUnderlying.Reset(new worker::ReserveEntityIdResponseOp());
}

UReserveEntityIdResponse* UReserveEntityIdResponse::Init(const worker::ReserveEntityIdResponseOp& underlying)
{
	mUnderlying.Reset(new worker::ReserveEntityIdResponseOp(underlying));
	return this;
}

worker::ReserveEntityIdResponseOp* UReserveEntityIdResponse::GetUnderlying()
{
	return mUnderlying.Get();
}

bool UReserveEntityIdResponse::WasSuccessful()
{
	return mUnderlying->StatusCode == worker::StatusCode::kSuccess;
}

int UReserveEntityIdResponse::GetEntityId()
{
	if (mUnderlying->EntityId.empty())
	{
		return -1;
	}
	return (int) *(mUnderlying->EntityId.data());
}