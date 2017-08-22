// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#pragma once

#include "EntityId.h"
#include "EntityTemplate.h"
#include "SpatialOS.h"
#include "RpgDemoGameMode.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FGetSpawnerEntityIdResultDelegate, bool, success, FString, errorMessage, FEntityId, spawnerEntityId);

UCLASS(minimalapi)
class ARpgDemoGameMode : public AGameModeBase
{
    GENERATED_BODY()

  public:
    ARpgDemoGameMode();

    UFUNCTION(BlueprintCallable, Category = "RpgDemoGameMode")
    FString GetSpatialOsWorkerType() const;

    UFUNCTION(BlueprintImplementableEvent, Category = "RpgDemoGameMode")
    void OnSpatialOsConnected();

    UFUNCTION(BlueprintImplementableEvent, Category = "RpgDemoGameMode")
    void OnSpatialOsDisconnected();

    UFUNCTION(BlueprintImplementableEvent, Category = "RpgDemoGameMode")
    void OnSpatialOsFailedToConnect();

    UFUNCTION(BlueprintPure, Category = "RpgDemoGameMode")
    UEntityTemplate* CreatePlayerEntityTemplate(FString clientWorkerId, const FVector& position);

    UFUNCTION(BlueprintCallable, Category = "RpgDemoGameMode")
    void GetSpawnerEntityId(const FGetSpawnerEntityIdResultDelegate& callback, int timeoutMs);

    void StartPlay() override;

    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintPure, Category = "RpgDemoGameMode")
    bool IsConnectedToSpatialOs() const;

    UFUNCTION(BlueprintPure, Category = "RpgDemoGameMode")
    UCommander* SendWorkerCommand();

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, NoClear)
    FString WorkerTypeOverride;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, NoClear)
    FString WorkerIdOverride;

    /*
    * Note: The flag UseExternalIp is required to connect to a deployment using
    * the terminal command `spatial connect`. It is also required when you launch
    * your worker using the SpatialOS launcher.
    */
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, NoClear)
    bool UseExternalIp;

  private:
    DECLARE_DELEGATE(FUnbindDelegate);

    USpatialOS* GetSpatialOS() const;

    UPROPERTY()
    UCommander* Commander;

    FGetSpawnerEntityIdResultDelegate* GetSpawnerEntityIdResultCallback;

    FUnbindDelegate UnbindEntityQueryDelegate;
    void UnbindEntityQueryCallback();
    std::uint64_t entityQueryCallback;
};