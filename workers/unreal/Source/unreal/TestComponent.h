// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "improbable/test/test.h"
#include "ScopedViewCallbacks.h"
#include "TestComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREAL_API UTestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTestComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void Init(worker::Connection& Connection, worker::View& View, worker::EntityId EntityId);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TestComponent")
	const unsigned int ComponentId = 1003;

	UPROPERTY(BlueprintPure, Category = "TestComponent")
		bool HasAuthority();

	UPROPERTY(BlueprintPure, Category = "TestComponent")
		bool IsComponentReady();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuthorityChangeDelegate, bool, newAuthority)
	UPROPERTY(BlueprintAssignable, Category = "TestComponent")
		FAuthorityChangeDelegate OnAuthorityChange;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FComponentReadyDelegate)
	UPROPERTY(BlueprintAssignable, Category = "TestComponent")
		FComponentReadyDelegate OnComponentReady;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentUpdateDelegate, improbable::test::TestState::Update, update)
	UPROPERTY(BlueprintAssignable, Category = "TestComponent")
		FComponentUpdateDelegate OnComponentUpdate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInt32ValDelegate, int32_t, newInt32Val)
	UPROPERTY(BlueprintAssignable, Category = "TestComponent")
		FInt32ValDelegate OnInt32ValUpdate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextEventDelegate, improbable::test::StringWrapper, textEvent)
	UPROPERTY(BlueprintAssignable, Category = "TestComponent")
		FTextEventDelegate OnTextEvent;

private:
	TAutoPtr<worker::Connection> mConnection;
	TAutoPtr<worker::View> mView;
	worker::EntityId mEntityId;
	TAutoPtr<improbable::unreal::callbacks::FScopedViewCallbacks> mCallbacks;

	bool mHasAuthority;
	bool mIsComponentReady;

	int32_t mInt32Val;

	void OnAuthorityChangeDispatcherCallback(const worker::AuthorityChangeOp& op);
	void OnAddComponentDispatcherCallback(const worker::AddComponentOp<improbable::test::TestState> op);
	void OnRemoveComponentDispatcherCallback(const worker::RemoveComponentOp op);
	void OnComponentUpdateDispatcherCallback(const worker::ComponentUpdateOp<improbable::test::TestState> op);
	void ApplyComponentUpdate(const improbable::test::TestState::Update update);
};
