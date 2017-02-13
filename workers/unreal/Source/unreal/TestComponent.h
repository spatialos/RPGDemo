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

	const unsigned int ComponentId = 1003;

	void Init(worker::Connection& Connection, worker::View& View);

private:
	//worker::Connection& Connection;
	//worker::View& View;
	//improbable::unreal::callbacks::FScopedViewCallbacks Callbacks;
};
