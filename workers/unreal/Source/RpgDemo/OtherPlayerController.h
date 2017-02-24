// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "RpgDemoCharacter.h"
#include "OtherPlayerController.generated.h"

/**
 *
 */
UCLASS()
class RPGDEMO_API AOtherPlayerController : public AAIController
{
    GENERATED_BODY()

public:
	AOtherPlayerController();
	virtual void Possess(APawn* InPawn) override;
	virtual void UnPossess() override;

protected:
	UFUNCTION(BlueprintCallable, Category = "OtherPlayerController")
	void OnPositionUpdate(FVector newSpatialOsPosition);

	void SetNewMoveDestination(const FVector& DestLocation);

	ARpgDemoCharacter* mOtherPlayer;
};
