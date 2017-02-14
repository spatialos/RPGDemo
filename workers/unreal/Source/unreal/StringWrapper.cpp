// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "StringWrapper.h"

UStringWrapper::UStringWrapper()
{
	mRawStringWrapper.Reset(new improbable::test::StringWrapper(""));
}

UStringWrapper::UStringWrapper(const improbable::test::StringWrapper& rawStringWrapper)
{
	mRawStringWrapper.Reset(new improbable::test::StringWrapper(rawStringWrapper));
}

FString UStringWrapper::GetStringVal()
{
	return FString(mRawStringWrapper->string_val().c_str());
}

void UStringWrapper::SetStringVal(FString stringVal)
{
	mRawStringWrapper->set_string_val(TCHAR_TO_UTF8(*stringVal));
}

improbable::test::StringWrapper* UStringWrapper::GetRawStringWrapper()
{
	return mRawStringWrapper.Get();
}