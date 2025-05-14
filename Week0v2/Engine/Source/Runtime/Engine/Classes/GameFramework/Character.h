#pragma once
#include "Pawn.h"

class USkeletalMeshComponent;
class UCapsuleShapeComponent;

class ACharacter : public APawn
{
    DECLARE_CLASS(ACharacter, APawn)
public:
    ACharacter();
    ACharacter(const ACharacter& Other);

    UObject* Duplicate(UObject* InOuter) override;
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;

protected:
    USkeletalMeshComponent* BodyMesh = nullptr;
    UCapsuleShapeComponent* CollisionCapsule = nullptr;
};
