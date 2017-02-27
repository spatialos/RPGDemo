// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SpatialOSGameMode.h"
#include "Improbable/Generated/cpp/unreal/EntityTemplate.h"
#include "RpgDemoGameMode.generated.h"

UCLASS(minimalapi)
class ARpgDemoGameMode : public ASpatialOSGameMode
{
	GENERATED_BODY()

public:
	ARpgDemoGameMode();
	virtual ~ARpgDemoGameMode();

	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "RpgDemoGameMode")
	UEntityTemplate* CreatePlayerEntityTemplate(FString clientWorkerId, const FVector& position);

	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FGetSpawnerEntityIdResultDelegate, bool, success, FString, errorMessage, int, spawnerEntityId);
	UFUNCTION(BlueprintCallable, Category = "RpgDemoGameMode")
	void GetSpawnerEntityId(const FGetSpawnerEntityIdResultDelegate& callback, int timeoutMs);

private:
	static ARpgDemoGameMode* Instance;
	FGetSpawnerEntityIdResultDelegate* mGetSpawnerEntityIdResultCallback;
};
