// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ScopedViewCallbacks.h"
#include "SpatialOsComponent.generated.h"

UCLASS(abstract)
class UNREAL_API USpatialOsComponent : public UActorComponent
{
    GENERATED_BODY()

  public:
    // Sets default values for this component's properties
    USpatialOsComponent();

    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    virtual void Init(worker::Connection& Connection, worker::View& View,
                      worker::EntityId EntityId);

    UFUNCTION(BlueprintPure, Category = "SpatialOsComponent")
    virtual int GetComponentId() PURE_VIRTUAL(USpatialOsComponent::GetComponentId, return 0;);

    //UFUNCTION(BlueprintPure, Category = "SpatialOsComponent")
    worker::EntityId GetEntityId();

    UFUNCTION(BlueprintPure, Category = "SpatialOsComponent")
    bool HasAuthority();

    UFUNCTION(BlueprintPure, Category = "SpatialOsComponent")
    bool IsComponentReady();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuthorityChangeDelegate, bool, newAuthority);
    UPROPERTY(BlueprintAssignable, Category = "SpatialOsComponent")
    FAuthorityChangeDelegate OnAuthorityChange;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FComponentReadyDelegate);
    UPROPERTY(BlueprintAssignable, Category = "TestComponent")
    FComponentReadyDelegate OnComponentReady;

  protected:
    worker::Connection* mConnection;
    worker::View* mView;
    worker::EntityId mEntityId;
    TUniquePtr<improbable::unreal::callbacks::FScopedViewCallbacks> mCallbacks;

    bool mHasAuthority;
    bool mIsComponentReady;

    void OnAuthorityChangeDispatcherCallback(const worker::AuthorityChangeOp& op);
    void OnRemoveComponentDispatcherCallback(const worker::RemoveComponentOp& op);
};
