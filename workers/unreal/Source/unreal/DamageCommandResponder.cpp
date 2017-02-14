// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "DamageCommandResponder.h"

UDamageCommandResponder::UDamageCommandResponder()
{
}

UDamageCommandResponder::UDamageCommandResponder(worker::Connection* Connection, uint32_t requestId, UDamageRequest* request)
{
	mRequestId = requestId;
	mRequest.Reset(request);
}

UDamageRequest* UDamageCommandResponder::GetRequest()
{
	return mRequest.Get();
}

void UDamageCommandResponder::SendResponse(UDamageResponse* response)
{

}