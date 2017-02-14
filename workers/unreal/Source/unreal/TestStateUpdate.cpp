// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "TestStateUpdate.h"

UTestStateUpdate::UTestStateUpdate()
{
	mRawUpdate.Reset(new improbable::test::TestState::Update());
}

UTestStateUpdate::UTestStateUpdate(const improbable::test::TestState::Update& rawUpdate)
{
	mRawUpdate.Reset(new improbable::test::TestState::Update(rawUpdate));
}

bool UTestStateUpdate::HasInt32Val()
{
	return !mRawUpdate->int32_val().empty();
}

int UTestStateUpdate::GetInt32Val()
{
	return (int) *(mRawUpdate->int32_val().data());
}

void UTestStateUpdate::SetInt32Val(int int32Val)
{
	mRawUpdate->set_int32_val(int32Val);
}