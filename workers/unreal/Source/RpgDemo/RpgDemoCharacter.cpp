// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "RpgDemo.h"
#include "ConversionsFunctionLibrary.h"
#include "EntitySpawner.h"
#include "Improbable/Generated/cpp/unreal/TransformComponent.h"
#include "OtherPlayerController.h"
#include "RPGDemoGameInstance.h"
#include "RpgDemoCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "SpatialOS.h"

ARpgDemoCharacter::ARpgDemoCharacter()
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

    // Create a transform component
    TransformComponent = CreateDefaultSubobject<UTransformComponent>(TEXT("TransformComponent"));

    SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

void ARpgDemoCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (TransformComponent->HasAuthority())
    {
        UpdateCursorPosition();

        const auto spatialOsPosition =
            UConversionsFunctionLibrary::UnrealCoordinatesToSpatialOsCoordinates(
                GetActorLocation());
        const auto rawUpdate =
            improbable::common::Transform::Update().set_position(improbable::math::Coordinates(
                spatialOsPosition.X, spatialOsPosition.Y, spatialOsPosition.Z));

        const auto update = NewObject<UTransformComponentUpdate>()->Init(rawUpdate);
        TransformComponent->SendComponentUpdate(update);
    }
}

void ARpgDemoCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitialiseAsOtherPlayer();
    TransformComponent->OnAuthorityChange.AddDynamic(
        this, &ARpgDemoCharacter::OnTransformAuthorityChange);
    TransformComponent->OnComponentReady.AddDynamic(this,
                                                    &ARpgDemoCharacter::OnTransformComponentReady);
}

void ARpgDemoCharacter::OnTransformAuthorityChange(bool newAuthority)
{
    Initialise(newAuthority);
}

void ARpgDemoCharacter::OnTransformComponentReady()
{
    const auto unrealPosition =
        UConversionsFunctionLibrary::SpatialOsCoordinatesToUnrealCoordinates(
            TransformComponent->GetPosition());
    SetActorLocation(unrealPosition);
}

/** If this is our player, then possess it with the player controller and activate the camera and
 *the cursor,
 *	otherwise, add an OtherPlayerController */
void ARpgDemoCharacter::Initialise(bool authority)
{
    if (authority)
    {
        InitialiseAsOwnPlayer();
        UE_LOG(LogTemp, Warning,
               TEXT("ARpgDemoCharacter::Initialise did just call InitialiseAsOwnPlayer"
                    "controller for actor %s"),
               *GetName())
    }
    else
    {
        InitialiseAsOtherPlayer();
        UE_LOG(LogTemp, Warning,
               TEXT("ARpgDemoCharacter::Initialise did just call InitialiseAsOtherPlayer"
                    "controller for actor %s"),
               *GetName())
    }
}

void ARpgDemoCharacter::InitialiseAsOwnPlayer()
{
    APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    if (playerController == nullptr)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("ARpgDemoCharacter::InitialiseAsOwnPlayer error, playerController was null"))
        return;
    }
    AController* currentController = GetController();
    if (currentController != playerController)
    {
        if (currentController != nullptr)
        {
            currentController->UnPossess();
        }

        if (playerController->GetPawn() != nullptr)
        {
            playerController->UnPossess();
        }
        playerController->Possess(this);
        UE_LOG(LogTemp, Warning,
               TEXT("ARpgDemoCharacter::InitialiseAsOwnPlayer creating own player "
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

void ARpgDemoCharacter::InitialiseAsOtherPlayer()
{
    AController* currentController = GetController();
    APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    if (currentController == playerController && playerController != nullptr)
    {
        playerController->UnPossess();
        currentController = GetController();
    }

    if (currentController == nullptr)
    {
        AOtherPlayerController* otherPlayerController = Cast<AOtherPlayerController>(
            GetWorld()->SpawnActor(AOtherPlayerController::StaticClass()));

        UE_LOG(LogTemp, Warning,
               TEXT("ARpgDemoCharacter::InitialiseAsOtherPlayer creating other player "
                    "controller for actor %s"),
               *GetName())
        otherPlayerController->Possess(this);
    }
    else
    {
        UE_LOG(LogTemp, Warning,
               TEXT("ARpgDemoCharacter::InitialiseAsOtherPlayer controller was not null"
                    "controller for actor %s"),
               *GetName())
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

void ARpgDemoCharacter::UpdateCursorPosition() const
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

int ARpgDemoCharacter::GetEntityId()
{
    auto GameInstance = Cast<URPGDemoGameInstance>(GetWorld()->GetGameInstance());

    if (GameInstance != nullptr)
    {
        auto EntitySpawner = GameInstance->GetEntitySpawner();
        if (EntitySpawner != nullptr)
        {
            return EntitySpawner->GetEntityId(this);
        }
    }
    return static_cast<int>(-1);
}