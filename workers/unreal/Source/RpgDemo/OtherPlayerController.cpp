// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "OtherPlayerController.h"
#include "Improbable/Generated/cpp/unreal/TransformComponent.h"
#include "RpgDemoCharacter.h"
#include "ConversionsFunctionLibrary.h"

AOtherPlayerController::AOtherPlayerController()
{
}

void AOtherPlayerController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	mOtherPlayer = Cast<ARpgDemoCharacter>(InPawn);
	mOtherPlayer->GetTransformComponent()->OnPositionUpdate.AddDynamic(this, &AOtherPlayerController::OnPositionUpdate);
}

void AOtherPlayerController::UnPossess()
{
	Super::UnPossess();
	mOtherPlayer->GetTransformComponent()->OnPositionUpdate.RemoveDynamic(this, &AOtherPlayerController::OnPositionUpdate);
}

void AOtherPlayerController::OnPositionUpdate(FVector newSpatialOsPosition)
{
	const auto newUnrealPosition = UConversionsFunctionLibrary::SpatialOsCoordinatesToUnrealCoordinates(newSpatialOsPosition);
	SetNewMoveDestination(newUnrealPosition);
}

void AOtherPlayerController::SetNewMoveDestination(const FVector& DestLocation)
{
    UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
	float const Distance = FVector::Dist(DestLocation, mOtherPlayer->GetActorLocation());
	if (NavSys && (Distance > 120.0f))
	{
		NavSys->SimpleMoveToLocation(this, DestLocation);
	}
}
