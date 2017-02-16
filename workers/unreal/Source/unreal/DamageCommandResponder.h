// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageRequest.h"
#include "DamageResponse.h"
#include "UObject/NoExportTypes.h"
#include "improbable/test/test.h"
#include "DamageCommandResponder.generated.h"

/**
 *
 */
UCLASS()
class UNREAL_API UDamageCommandResponder : public UObject
{
    GENERATED_BODY()

  public:
    UDamageCommandResponder();
    UDamageCommandResponder(
        worker::Connection* connection,
        worker::RequestId<
            worker::IncomingCommandRequest<improbable::test::TestState::Commands::Damage>>
            requestId,
        UDamageRequest* request);

    UFUNCTION(BlueprintPure, Category = "TestComponent")
    UDamageRequest* GetRequest();

    UFUNCTION(BlueprintCallable, Category = "TestComponent")
    void SendResponse(UDamageResponse* response);

  private:
    worker::Connection* mConnection;
    worker::RequestId<worker::IncomingCommandRequest<improbable::test::TestState::Commands::Damage>>
        mRequestId;
    TUniquePtr<UDamageRequest> mRequest;
};
