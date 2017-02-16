// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageRequest.h"
#include "unreal.h"

UDamageRequest::UDamageRequest()
{
    mRawDamageRequest.Reset(new improbable::test::DamageRequest(0));
}

UDamageRequest::UDamageRequest(const improbable::test::DamageRequest& rawDamageRequest)
{
    mRawDamageRequest.Reset(new improbable::test::DamageRequest(rawDamageRequest));
}

int UDamageRequest::GetAmount()
{
    return (int)mRawDamageRequest->amount();
}

void UDamageRequest::SetAmount(int amount)
{
    mRawDamageRequest->set_amount((std::uint32_t)amount);
}