// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "unrealCharacter.generated.h"

UCLASS(Blueprintable)
class AunrealCharacter : public ACharacter
{
    GENERATED_BODY()

  public:
    AunrealCharacter();

    // Called every frame.
    virtual void Tick(float DeltaSeconds) override;

    virtual void BeginPlay() override;

    /** Returns TopDownCameraComponent subobject **/
    FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const
    {
        return TopDownCameraComponent;
    }
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const
    {
        return CameraBoom;
    }

    FORCEINLINE class UTransformReceiver* GetTransformReceiver() const
    {
        return TransformReceiver;
    }

  private:
    void UpdateCursorPosition() const;
    void Initialise();
    void InitialiseAsOwnPlayer();
    void InitialiseAsOtherPlayer();

    /** Top down camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera,
              meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* TopDownCameraComponent;

    /** Camera boom positioning the camera above the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera,
              meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** A decal that projects to the cursor location. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera,
              meta = (AllowPrivateAccess = "true"))
    class UDecalComponent* CursorToWorld;

    /** The transform receiver */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpatialOS",
              meta = (AllowPrivateAccess = "true"))
    class UTransformReceiver* TransformReceiver;

    /** The transform sender */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpatialOS",
              meta = (AllowPrivateAccess = "true"))
    class UTransformSender* TransformSender;
};
