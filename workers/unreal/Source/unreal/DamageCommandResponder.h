// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "DamageRequest.h"
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
	UDamageCommandResponder(uint32_t requestId, UDamageRequest* request);
	
	UFUNCTION(BlueprintPure, Category = "TestComponent")
		UDamageRequest* GetRequest();

	UFUNCTION(BlueprintCallable, Category = "TestComponent")
		void SendResponse(UDamageResponse* response);

private:
	uint32_t mRequestId;
	TUniquePtr<UDamageRequest> mRequest;
};
