// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "DamageCommandResponder.h"

UDamageCommandResponder::UDamageCommandResponder()
{
}

UDamageCommandResponder* UDamageCommandResponder::Init(
    worker::Connection* connection,
    worker::RequestId<worker::IncomingCommandRequest<improbable::test::TestState::Commands::Damage>>
        requestId,
    UDamageRequest* request)
{
    mConnection = connection;
    mRequestId = requestId;
    mRequest = request;
	return this;
}

UDamageRequest* UDamageCommandResponder::GetRequest()
{
	return mRequest;
}

void UDamageCommandResponder::SendResponse(UDamageResponse* response)
{
	improbable::test::DamageResponse rawResponse = response->GetUnderlying();
    mConnection->SendCommandResponse(
        mRequestId, improbable::test::TestState::Commands::Damage::Response(rawResponse));
}