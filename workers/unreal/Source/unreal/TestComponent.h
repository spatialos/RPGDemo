// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageCommandResponder.h"
#include "SpatialOsComponent.h"
#include "StringWrapper.h"
#include "TestStateUpdate.h"
#include "improbable/test/test.h"
#include "TestComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREAL_API UTestComponent : public USpatialOsComponent
{
    GENERATED_BODY()

  public:
    // Sets default values for this component's properties
    UTestComponent();

    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintPure, Category = "TestComponent")
    int GetComponentId();

    UFUNCTION(BlueprintPure, Category = "TestComponent")
    int GetInt32Val();

    UFUNCTION(BlueprintCallable, Category = "TestComponent")
    void SendComponentUpdate(UTestStateUpdate* update);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentUpdateDelegate, UTestStateUpdate*,
                                                update);
    UPROPERTY(BlueprintAssignable, Category = "TestComponent")
    FComponentUpdateDelegate OnComponentUpdate;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInt32ValDelegate, int, newInt32Val);
    UPROPERTY(BlueprintAssignable, Category = "TestComponent")
    FInt32ValDelegate OnInt32ValUpdate;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextEventDelegate, UStringWrapper*, textEvent);
    UPROPERTY(BlueprintAssignable, Category = "TestComponent")
    FTextEventDelegate OnTextEvent;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamageCommandRequestDelegate,
                                                UDamageCommandResponder*, responder);
    UPROPERTY(BlueprintAssignable, Category = "TestComponent")
    FDamageCommandRequestDelegate OnDamageCommandRequest;

  private:
    const int mComponentId = 1003;

    int mInt32Val;

    void
    OnAddComponentDispatcherCallback(const worker::AddComponentOp<improbable::test::TestState>& op);

    void OnComponentUpdateDispatcherCallback(
        const worker::ComponentUpdateOp<improbable::test::TestState>& op);
    void ApplyComponentUpdate(const improbable::test::TestState::Update& update);

    void OnDamageCommandRequestDispatcherCallback(
        const worker::CommandRequestOp<improbable::test::TestState::Commands::Damage>& op);
};
