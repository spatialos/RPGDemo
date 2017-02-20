// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
// ===========
// DO NOT EDIT - this file is automatically regenerated.
// =========== 

#pragma once

#include "UObject/NoExportTypes.h"
#include "ScopedViewCallbacks.h"
#include "ReserveEntityIdResponse.h"
#include "Commander.generated.h"

class USpatialOsComponent;

/**
*
*/
UCLASS()
class UCommander : public UObject
{
	GENERATED_BODY()

public:
	UCommander();
	UCommander* Init(USpatialOsComponent* component, worker::Connection& connection, worker::View& view);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReserveEntityIdResponse, UReserveEntityIdResponse*, commandResponse);
	UFUNCTION(BlueprintCallable, Category = "Commands")
	void ReserveEntityId(const FReserveEntityIdResponse& callback, int timeoutMs);

private:
	worker::Connection* mConnection;
	worker::View* mView;
	USpatialOsComponent* mComponent;

	TUniquePtr<improbable::unreal::callbacks::FScopedViewCallbacks> mCallbacks;
	std::set<std::string> mCommandResponseDispatcherCallbacksRegistered;
	std::map<std::uint32_t, FReserveEntityIdResponse> mRequestIdToReserveEntityIdCallback;

	void OnReserveEntityIdResponseDispatcherCallback(const worker::ReserveEntityIdResponseOp& op);
};
