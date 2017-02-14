// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "improbable/test/test.h"
#include "StringWrapper.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_API UStringWrapper : public UObject
{
	GENERATED_BODY()

public:
	UStringWrapper();
	UStringWrapper(const improbable::test::StringWrapper& rawStringWrapper);
	
	UFUNCTION(BlueprintPure, Category = "StringWrapper")
		FString GetStringVal();

	UFUNCTION(BlueprintCallable, Category = "StringWrapper")
		void SetStringVal(FString stringVal);

	improbable::test::StringWrapper* GetRawStringWrapper();

private:
	TUniquePtr<improbable::test::StringWrapper> mRawStringWrapper;
};
