// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"

#include "SpatialOSConversionFunctionLibrary.h"
#include "Improbable/Generated/cpp/unreal/PositionComponent.h"
#include "OtherPlayerController.h"
#include "RpgDemoCharacter.h"

AOtherPlayerController::AOtherPlayerController()
{
}

void AOtherPlayerController::Possess(APawn* InPawn)
{
    Super::Possess(InPawn);
    mOtherPlayer = Cast<ARpgDemoCharacter>(InPawn);

    mOtherPlayer->GetPositionComponent()->OnCoordsUpdate.AddDynamic(
        this, &AOtherPlayerController::OnPositionUpdate);
}

void AOtherPlayerController::UnPossess()
{
    Super::UnPossess();
    mOtherPlayer->GetPositionComponent()->OnCoordsUpdate.RemoveDynamic(
        this, &AOtherPlayerController::OnPositionUpdate);
}

void AOtherPlayerController::OnPositionUpdate(FVector newSpatialOsPosition)
{
    const auto newUnrealPosition =
		USpatialOSConversionFunctionLibrary::SpatialOsCoordinatesToUnrealCoordinates(newSpatialOsPosition);
    SetNewMoveDestination(newUnrealPosition);
}

void AOtherPlayerController::SetNewMoveDestination(const FVector& DestLocation)
{
    UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
    float const Distance = FVector::Dist(DestLocation, mOtherPlayer->GetActorLocation());
    if (NavSys)
    {
        NavSys->SimpleMoveToLocation(this, DestLocation);
    }
}
