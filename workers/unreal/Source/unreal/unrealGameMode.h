// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "GameFramework/GameUserSettings.h"
#include "EntitySpawner.h"
#include "WorkerConnection.h"
#include "unrealGameMode.generated.h"

UCLASS(minimalapi)
class AunrealGameMode : public AGameMode
{
  GENERATED_BODY()

public:
  AunrealGameMode();
  virtual ~AunrealGameMode();

  static improbable::unreal::entity_spawning::FEntitySpawner* GetSpawner()
  {
    return Instance->Spawner.GetOwnedPointer();
  }

private:
  void StartPlay() override;
  void Tick(float DeltaTime) override;

  void SpawnPlayer();

  TAutoPtr<improbable::unreal::core::FWorkerConnection> Connection;
  TAutoPtr<improbable::unreal::entity_spawning::FEntitySpawner> Spawner;

  static void ConfigureWindowSize();
  void CreateWorkerConnection();
  void RegisterEntityBlueprints();

  static void MakeWindowed(int32 Width, int32 Height);
  static UGameUserSettings* GetGameUserSettings();

  static AunrealGameMode* Instance;
};