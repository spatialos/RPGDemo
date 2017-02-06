// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "unreal.h"
#include "OtherPlayerController.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "TransformReceiver.h"
#include "TransformSender.h"
#include "unrealCharacter.h"

AunrealCharacter::AunrealCharacter()
{
    // Set size for player capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate character to camera direction
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement =
        true;  // Rotate character to moving direction
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    // Create a camera boom...
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bAbsoluteRotation = true;  // Don't want arm to rotate when character does
    CameraBoom->TargetArmLength = 800.f;
    CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
    CameraBoom->bDoCollisionTest =
        false;  // Don't want to pull camera in when it collides with level

    // Create a camera...
    TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCameraComponent->bUsePawnControlRotation =
        false;  // Camera does not rotate relative to arm

    // Create a decal in the world to show the cursor's location
    CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
    CursorToWorld->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(
        TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
    if (DecalMaterialAsset.Succeeded())
    {
        CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
    }
    CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
    CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

    // Deactivate by default
    CursorToWorld->SetActive(false);
    CursorToWorld->SetHiddenInGame(true);

    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // Create a transform receiver
    TransformReceiver = CreateDefaultSubobject<UTransformReceiver>(TEXT("TransformReceiver"));

    // Create a transform sender
    TransformSender = CreateDefaultSubobject<UTransformSender>(TEXT("TransformSender"));

    SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

void AunrealCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    Initialise();

#if !UE_SERVER
    if (TransformSender->HasAuthority())
    {
        UpdateCursorPosition();
    }
#endif
}

/** If this is our player, then possess it with the player controller and activate the camera and
 *the cursor,
 *	otherwise, add an OtherPlayerController */
void AunrealCharacter::Initialise()
{
#if !UE_SERVER
    if (TransformSender->HasAuthority())
    {
        InitialiseAsOwnPlayer();
    }
    else
#endif
    {
        InitialiseAsOtherPlayer();
    }
}

void AunrealCharacter::InitialiseAsOwnPlayer()
{
    APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    AController* currentController = GetController();
    if (currentController != playerController)
    {
        if (currentController != nullptr)
        {
            currentController->UnPossess();
        }

        playerController->UnPossess();
        playerController->Possess(this);
        UE_LOG(LogTemp, Warning, TEXT("AunrealCharacter::InitialiseAsOwnPlayer creating own player "
                                      "controller for actor %s"),
               *GetName())
    }

    if (!CursorToWorld->IsActive())
    {
        CursorToWorld->SetActive(true);
    }
    CursorToWorld->SetHiddenInGame(false);

    if (!TopDownCameraComponent->IsActive())
    {
        TopDownCameraComponent->Activate();
    }
}

void AunrealCharacter::InitialiseAsOtherPlayer()
{
    AController* currentController = GetController();

#if !UE_SERVER
    APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    if (currentController == playerController)
    {
        playerController->UnPossess();
        currentController = GetController();
    }
#endif

    if (currentController == nullptr)
    {
        AOtherPlayerController* otherPlayerController = Cast<AOtherPlayerController>(
            GetWorld()->SpawnActor(AOtherPlayerController::StaticClass()));

        UE_LOG(LogTemp, Warning,
               TEXT("AunrealCharacter::InitialiseAsOtherPlayer creating other player "
                    "controller for actor %s"),
               *GetName())
        otherPlayerController->Possess(this);
    }

    if (CursorToWorld->IsActive())
    {
        CursorToWorld->SetActive(false);
    }
    CursorToWorld->SetHiddenInGame(true);

    if (TopDownCameraComponent->IsActive())
    {
        TopDownCameraComponent->Deactivate();
    }
}

void AunrealCharacter::UpdateCursorPosition() const
{
    if (CursorToWorld != nullptr)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            FHitResult TraceHitResult;
            PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
            FVector CursorFV = TraceHitResult.ImpactNormal;
            FRotator CursorR = CursorFV.Rotation();
            CursorToWorld->SetWorldLocation(TraceHitResult.Location);
            CursorToWorld->SetWorldRotation(CursorR);
        }
    }
}
