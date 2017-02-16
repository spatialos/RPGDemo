// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "DamageResponse.h"

UDamageResponse::UDamageResponse()
{
    mRawDamageResponse.Reset(new improbable::test::DamageResponse(0));
}

UDamageResponse::UDamageResponse(const improbable::test::DamageResponse& rawDamageResponse)
{
    mRawDamageResponse.Reset(new improbable::test::DamageResponse(rawDamageResponse));
}

int UDamageResponse::GetDealtDamage()
{
    return (int)mRawDamageResponse->dealt_damage();
}

UDamageResponse* UDamageResponse::SetDealtDamage(int dealtDamage)
{
    mRawDamageResponse->set_dealt_damage((std::uint32_t)dealtDamage);
    return this;
}

improbable::test::DamageResponse* UDamageResponse::GetRawDamageResponse()
{
    return mRawDamageResponse.Get();
}