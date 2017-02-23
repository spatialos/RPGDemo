// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SpatialOSGameMode.h"
#include "EntitySpawner.h"
#include "RpgDemoGameMode.generated.h"

UCLASS(minimalapi)
class ARpgDemoGameMode : public ASpatialOSGameMode
{
	GENERATED_BODY()

public:
	ARpgDemoGameMode();
	virtual ~ARpgDemoGameMode();

	static improbable::unreal::entity_spawning::FEntitySpawner* GetSpawner()
	{
		return Instance->Spawner.Get();
	}

	void Tick(float DeltaTime) override;

protected:

	UFUNCTION(BlueprintCallable, Category = "Worker Startup")
	void SpawnPlayer();

	UFUNCTION(BlueprintCallable, Category = "Worker Startup")
	void RegisterEntityBlueprints();

private:
	TUniquePtr<improbable::unreal::entity_spawning::FEntitySpawner> Spawner;

	static ARpgDemoGameMode* Instance;
};
