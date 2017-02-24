// Fill out your copyright notice in the Description page of Project Settings.

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
	UE_LOG(LogTemp, Warning,
			TEXT("AOtherPlayerController::OnPositionUpdate updating position for other player"))
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
