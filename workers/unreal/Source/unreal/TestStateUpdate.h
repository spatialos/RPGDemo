// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "improbable/test/test.h"
#include "TestStateUpdate.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_API UTestStateUpdate : public UObject
{
	GENERATED_BODY()
	
public:
	UTestStateUpdate();
	UTestStateUpdate(const improbable::test::TestState::Update& rawUpdate);
	
	UFUNCTION(BlueprintPure, Category = "TestComponent")
		bool HasInt32Val();

	UFUNCTION(BlueprintPure, Category = "TestComponent")
		int GetInt32Val();

	UFUNCTION(BlueprintCallable, Category = "TestComponent")
		void SetInt32Val(int int32Val);

	const improbable::test::TestState::Update GetRawUpdate();

private:
	TUniquePtr<improbable::test::TestState::Update> mRawUpdate;
};
