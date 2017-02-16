// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "DamageCommandResponder.h"

UDamageCommandResponder::UDamageCommandResponder()
{
}

void UDamageCommandResponder::Init(
    worker::Connection* connection,
    worker::RequestId<worker::IncomingCommandRequest<improbable::test::TestState::Commands::Damage>>
        requestId,
    UDamageRequest* request)
{
    mConnection = connection;
    mRequestId = requestId;
    mRequest.Reset(request);
}

UDamageRequest* UDamageCommandResponder::GetRequest()
{
    return mRequest.Get();
}

void UDamageCommandResponder::SendResponse(UDamageResponse* response)
{
    auto rawResponse = response->GetRawDamageResponse();
    mConnection->SendCommandResponse(
        mRequestId, improbable::test::TestState::Commands::Damage::Response(*rawResponse));
}