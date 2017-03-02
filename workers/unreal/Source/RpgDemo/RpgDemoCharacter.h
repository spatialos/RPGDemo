// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#pragma once
#include "GameFramework/Character.h"
#include "RpgDemoCharacter.generated.h"

UCLASS(Blueprintable)
class ARpgDemoCharacter : public ACharacter
{
    GENERATED_BODY()

  public:
    ARpgDemoCharacter();

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

    FORCEINLINE class UTransformComponent* GetTransformComponent() const
    {
        return TransformComponent;
    }

    UFUNCTION(BlueprintPure, Category = "RpgDemoCharacter")
    int GetEntityId();

  private:
    void UpdateCursorPosition() const;
    void Initialise(bool authority);
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

    /** The transform component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RpgDemoCharacter",
              meta = (AllowPrivateAccess = "true"))
    class UTransformComponent* TransformComponent;

	UFUNCTION(BlueprintCallable, Category = "RpgDemoCharacter")
	void OnTransformComponentReady();

    UFUNCTION(BlueprintCallable, Category = "RpgDemoCharacter")
    void OnTransformAuthorityChange(bool newAuthority);
};
