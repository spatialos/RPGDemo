// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
// ===========
// DO NOT EDIT - this file is automatically regenerated.
// =========== 

#pragma once

#include "UObject/NoExportTypes.h"
#include "improbable/worker.h"
#include "ReserveEntityIdResponse.generated.h"

/**
*
*/
UCLASS()
class UReserveEntityIdResponse : public UObject
{
	GENERATED_BODY()

public:
	UReserveEntityIdResponse();
	UReserveEntityIdResponse* Init(const worker::ReserveEntityIdResponseOp& underlying);

	worker::ReserveEntityIdResponseOp* GetUnderlying();

	UFUNCTION(BlueprintPure, Category = "ReserveEntityIdCommand")
		bool WasSuccessful();

	UFUNCTION(BlueprintPure, Category = "ReserveEntityIdCommand")
		int GetEntityId();

private:
	TUniquePtr<worker::ReserveEntityIdResponseOp> mUnderlying;
};