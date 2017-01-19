// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include <improbable/worker.h>
#include "SpatialOS/Public/SpatialEntityStorageComponent.h"
#include "ScopedViewCallbacks.h"
#include "improbable/corelibrary/transforms/transform_state.h"
#include "TransformReceiver.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTransformReceiver : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTransformReceiver();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, Category = "SpatialOS")
		FVector GetLocation() const;

	UFUNCTION(BlueprintCallable, Category = "SpatialOS")
		FQuat GetRotation() const;

private:

	USpatialEntityStorageComponent* mSpatialComponent;
	TAutoPtr<improbable::unreal::callbacks::FScopedViewCallbacks> mCallbacks;
	void OnTransformComponentUpdate(const worker::ComponentUpdateOp<improbable::corelibrary::transforms::TransformState>& op);
	void ParseTransformStateUpdate(const worker::ComponentUpdateOp<improbable::corelibrary::transforms::TransformState>& op);
	void Initialise();
	bool IsInitialised() const;
	worker::Entity* GetEntity() const;

	FVector mLocation;
	FQuat mRotation;
};
