#pragma once
#include "Pawn.h"

class UCapsuleShapeComponent;
class UStaticMeshComponent;

class ACharacter : public APawn
{
    DECLARE_CLASS(ACharacter, APawn)
public:
    ACharacter() = default;
    ACharacter(const ACharacter& Other);
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Destroyed() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UObject* Duplicate(UObject* InOuter) override;

    // virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;

protected:
    UStaticMeshComponent* BodyMesh = nullptr;
    UCapsuleShapeComponent* CollisionCapsule = nullptr;
};
