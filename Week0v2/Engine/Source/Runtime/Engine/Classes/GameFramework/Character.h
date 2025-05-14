#pragma once
#include "Pawn.h"

class UProjectileMovementComponent;
class USkeletalMeshComponent;
class UCapsuleShapeComponent;

class ACharacter : public APawn
{
    DECLARE_CLASS(ACharacter, APawn)
    DECLARE_MULTICAST_DELEGATE(PlayAnimAEvent);
    DECLARE_MULTICAST_DELEGATE(PlayAnimBEvent);
    DECLARE_MULTICAST_DELEGATE(PlayAnimCEvent);
public:
    ACharacter();
    ACharacter(const ACharacter& Other);

    UObject* Duplicate(UObject* InOuter) override;
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;

    void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;
    
    UProjectileMovementComponent* GetMovementComponent() const { return MovementComponent; }

    PlayAnimAEvent PlayAnimA;
    PlayAnimBEvent PlayAnimB;
    PlayAnimCEvent PlayAnimC;
    
protected:
    USkeletalMeshComponent* BodyMesh = nullptr;
    UCapsuleShapeComponent* CollisionCapsule = nullptr;
    UProjectileMovementComponent* MovementComponent = nullptr;
};
