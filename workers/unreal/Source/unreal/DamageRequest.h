// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "improbable/test/test.h"
#include "DamageRequest.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_API UDamageRequest : public UObject
{
	GENERATED_BODY()
	
public:
	UDamageRequest();
	UDamageRequest(const improbable::test::DamageRequest& rawDamageRequest);

	UFUNCTION(BlueprintPure, Category = "DamageRequest")
		int GetAmount();

	UFUNCTION(BlueprintCallable, Category = "DamageRequest")
		void SetAmount(int amount);
	
private:
	TUniquePtr<improbable::test::DamageRequest> mRawDamageRequest;
};
