// Fill out your copyright notice in the Description page of Project Settings.

#include "unreal.h"
#include "TransformReceiver.h"
#include "OtherPlayerController.h"
#include "unrealCharacter.h"

AOtherPlayerController::AOtherPlayerController()
{
	mControlledCharacter = nullptr;
}

void AOtherPlayerController::Initialise()
{
	if (GetPawn() == nullptr) return;
	mControlledCharacter = Cast<AunrealCharacter>(GetPawn());
}

bool AOtherPlayerController::IsInitialised() const
{
	return mControlledCharacter != nullptr;
}

void AOtherPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Initialise();
	if (!IsInitialised()) return;

	SetNewMoveDestination(mControlledCharacter->GetTransformReceiver()->GetLocation());
}

void AOtherPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// Issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}
