// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#pragma once

#include "Improbable/Generated/cpp/unreal/EntityTemplate.h"
#include "SpatialOSGameMode.h"
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

    // clang-format off
    DECLARE_DYNAMIC_DELEGATE_ThreeParams(FGetSpawnerEntityIdResultDelegate, bool, success, FString, errorMessage, int, spawnerEntityId);
    // clang-format on    
    UFUNCTION(BlueprintCallable, Category = "RpgDemoGameMode")
    void GetSpawnerEntityId(const FGetSpawnerEntityIdResultDelegate& callback, int timeoutMs);

  private:
    DECLARE_DELEGATE(FUnbindDelegate);

    static ARpgDemoGameMode* Instance;
    FGetSpawnerEntityIdResultDelegate* mGetSpawnerEntityIdResultCallback;

    FUnbindDelegate UnbindEntityQueryDelegate;
    void UnbindEntityQueryCallback();
    std::uint64_t entityQueryCallback;
};
